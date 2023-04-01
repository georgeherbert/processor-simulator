#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "memory_buffers.h"
#include "reg_file.h"
#include "reg.h"
#include "decoded_inst.h"

struct memory_buffers *memory_buffers_init(
    uint32_t num_buffers,
    uint32_t id_offset,
    struct reg_file *reg_file,
    struct cdb *cdb,
    struct reg *memory_buffers_all_busy,
    struct reg *memory_buffers_ready_address,
    struct reg *memory_buffers_ready_memory,
    struct rob *rob)
{
    struct memory_buffers *mb = malloc(sizeof(struct memory_buffers));
    if (mb == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for memory buffers");
        exit(EXIT_FAILURE);
    }

    mb->buffers_current = malloc(sizeof(struct memory_buffer) * num_buffers);
    if (mb->buffers_current == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for current individual memory buffers");
        exit(EXIT_FAILURE);
    }

    mb->buffers_next = malloc(sizeof(struct memory_buffer) * num_buffers);
    if (mb->buffers_next == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for next individual memory buffers");
        exit(EXIT_FAILURE);
    }

    mb->num_buffers = num_buffers;
    mb->reg_file = reg_file;
    mb->cdb = cdb;
    mb->memory_buffers_all_busy = memory_buffers_all_busy;
    mb->memory_buffers_ready_memory = memory_buffers_ready_memory;
    mb->memory_buffers_ready_address = memory_buffers_ready_address;
    mb->num_buffers_in_queue_current = 0;
    mb->num_buffers_in_queue_next = 0;
    mb->rob = rob;

    for (uint32_t i = 0; i < num_buffers; i++)
    {
        mb->buffers_current[i].id = i + id_offset;
        mb->buffers_current[i].busy = false;
        mb->buffers_next[i].id = i + id_offset;
        mb->buffers_next[i].busy = false;
        mb->buffers_current[i].queue_pos = 0;
        mb->buffers_next[i].queue_pos = 0;
    }

    return mb;
}

void memory_buffers_step(struct memory_buffers *mb)
{
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (mb->buffers_next[i].busy)
        {
            if (mb->buffers_next[i].qj != 0)
            {
                if (cdb_is_val_ready(mb->cdb, mb->buffers_next[i].qj))
                {
                    mb->buffers_next[i].vj = cdb_get_val(mb->cdb, mb->buffers_next[i].qj);
                    mb->buffers_next[i].qj = 0;
                }
            }
            if (mb->buffers_next[i].qk != 0)
            {
                if (cdb_is_val_ready(mb->cdb, mb->buffers_next[i].qk))
                {
                    mb->buffers_next[i].vk = cdb_get_val(mb->cdb, mb->buffers_next[i].qk);
                    mb->buffers_next[i].qk = 0;
                }
            }
        }
    }

    bool all_busy = true;
    bool ready_address = false;
    bool ready_memory = false;

    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        all_busy &= mb->buffers_next[i].busy;
    }

    if (mb->num_buffers_in_queue_next > 0)
    {
        uint32_t queue_pos_indices[mb->num_buffers_in_queue_next];
        for (uint32_t i = 0; i < mb->num_buffers; i++)
        {
            if (mb->buffers_next[i].busy && mb->buffers_next[i].queue_pos != 0)
            {
                queue_pos_indices[mb->buffers_next[i].queue_pos - 1] = i;
            }
        }

        /*
            A store is ready for effective address execution if:
                - qj == 0 (i.e. the "base" value is in Vj)
                - Store is at the front of the queue
        */
        struct memory_buffer *first_entry = &mb->buffers_next[queue_pos_indices[0]];
        bool store_seen = (first_entry->op == SW || first_entry->op == SH || first_entry->op == SB);
        if (store_seen && first_entry->qj == 0)
        {
            ready_address = true;
            mb->ready_address = first_entry;
        }

        /*
            A load is ready for effective address execution if:
                - qj == 0 (i.e. the "base" value is in Vj)
                - There are no prior stores in the queue for effective address calculation
        */
        uint32_t i = 0;
        while (!ready_address && !store_seen && i < mb->num_buffers_in_queue_next)
        {
            struct memory_buffer *entry = &mb->buffers_next[queue_pos_indices[i]];
            store_seen |= (entry->op == SW || entry->op == SH || entry->op == SB);
            if (!store_seen && entry->qj == 0)
            {
                ready_address = true;
                mb->ready_address = entry;
            }
            i++;
        }
    }
    /*
        A load is ready for memory execution if:
            - queue_pos == 0 (i.e. the effective address has been computed)
            - All stores earlier in the ROB have a different address
    */
    uint32_t i = 0;
    while (!ready_memory && i < mb->num_buffers)
    {
        struct memory_buffer *entry = &mb->buffers_next[i];
        if (entry->busy && entry->queue_pos == 0 && (entry->op == LW || entry->op == LH || entry->op == LHU || entry->op == LB || entry->op == LBU))
        {
            uint32_t rob_id = entry->rob_id;
            bool earlier_stores = rob_earlier_stores(mb->rob, rob_id, entry->a);
            if (!earlier_stores)
            {
                ready_memory = true;
                mb->ready_memory = entry;
            }
        }
        i++;
    }

    reg_write(mb->memory_buffers_all_busy, all_busy);
    reg_write(mb->memory_buffers_ready_memory, ready_memory);
    reg_write(mb->memory_buffers_ready_address, ready_address);
}

void memory_buffers_enqueue(
    struct memory_buffers *mb,
    enum op op,
    uint32_t qj,
    uint32_t qk,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t rob_id)
{
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (!mb->buffers_next[i].busy)
        {
            mb->buffers_next[i].busy = true;
            mb->buffers_next[i].op = op;
            mb->buffers_next[i].qj = qj;
            mb->buffers_next[i].qk = qk;
            mb->buffers_next[i].vj = vj;
            mb->buffers_next[i].vk = vk;
            mb->buffers_next[i].a = a;
            mb->buffers_next[i].rob_id = rob_id;
            /*
                The +1 in the next line ensures that the queue position is never 0.
                This is because 0 is used to indicate that the buffer is not in the queue.
            */
            mb->buffers_next[i].queue_pos = mb->num_buffers_in_queue_current + 1;
            mb->num_buffers_in_queue_next++;
            break;
        }
    }
}

// TODO: Change the name of this function
struct memory_buffer *memory_buffers_remove(struct memory_buffers *mb)
{
    // TODO: This is a pointer to the next queue, which is why I don't like it
    return mb->ready_memory;
}

void memory_buffers_set_buffer_not_busy(struct memory_buffers *mb, uint32_t id)
{
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (mb->buffers_next[i].id == id)
        {
            mb->buffers_next[i].busy = false;
            break;
        }
    }
}

// TODO: Change the name of this function
struct memory_buffer memory_buffers_dequeue(struct memory_buffers *mb)
{
    uint32_t pos = mb->ready_address->queue_pos;

    mb->num_buffers_in_queue_next--;

    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (mb->buffers_next[i].busy && mb->buffers_next[i].queue_pos > pos)
        {
            mb->buffers_next[i].queue_pos--;
        }
    }

    // TODO: This is a pointer to the next queue, which is why I don't like it
    return *mb->ready_address;
}

void memory_buffers_add_address(struct memory_buffers *mb, uint32_t id, uint32_t address)
{
    uint32_t id_index = 0;
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (mb->buffers_next[i].id == id)
        {
            id_index = i;
            break;
        }
    }
    mb->buffers_next[id_index].a = address;
    mb->buffers_next[id_index].queue_pos = 0;
}

void memory_buffers_update_current(struct memory_buffers *mb)
{
    memcpy(mb->buffers_current, mb->buffers_next, sizeof(struct memory_buffer) * mb->num_buffers);
    mb->num_buffers_in_queue_current = mb->num_buffers_in_queue_next;
}

void memory_buffers_destroy(struct memory_buffers *mb)
{
    free(mb->buffers_current);
    free(mb->buffers_next);
    free(mb);
}
