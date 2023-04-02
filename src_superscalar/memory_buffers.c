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
        if (!mb->buffers_current[i].busy)
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

bool memory_buffers_all_busy(struct memory_buffers *mb)
{
    bool all_busy = true;
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        all_busy &= mb->buffers_current[i].busy;
    }
    return all_busy;
}

struct memory_buffer *memory_buffers_dequeue_memory(struct memory_buffers *mb)
{
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        struct memory_buffer *mb_entry = &mb->buffers_current[i];
        enum op op = mb_entry->op;
        bool is_load = op == LW || op == LH || op == LHU || op == LB || op == LBU;
        if (is_load && mb_entry->busy && mb_entry->queue_pos == 0)
        {
            // We nest this if statement for efficiency
            if (!rob_earlier_stores(mb->rob, mb_entry->rob_id, mb_entry->a))
            {
                mb->buffers_next[i].busy = false;
                return mb_entry;
            }
        }
    }
    return NULL;
}

void get_queue_pos_indices(struct memory_buffers *mb, uint32_t *queue_pos_indices)
{
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (mb->buffers_current[i].busy && mb->buffers_current[i].queue_pos != 0)
        {
            queue_pos_indices[mb->buffers_current[i].queue_pos - 1] = i;
        }
    }
}

void shift_queue(struct memory_buffers *mb, uint32_t removed_pos)
{
    mb->num_buffers_in_queue_next--;
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (mb->buffers_next[i].busy && mb->buffers_next[i].queue_pos > removed_pos)
        {
            mb->buffers_next[i].queue_pos--;
        }
    }
}

struct memory_buffer *memory_buffers_dequeue_address(struct memory_buffers *mb)
{
    if (mb->num_buffers_in_queue_current > 0)
    {
        uint32_t queue_pos_indices[mb->num_buffers_in_queue_current];
        get_queue_pos_indices(mb, queue_pos_indices);

        struct memory_buffer *first_mb_entry = &mb->buffers_current[queue_pos_indices[0]];
        if ((first_mb_entry->op == SW || first_mb_entry->op == SH || first_mb_entry->op == SB))
        {
            if (first_mb_entry->qj == 0)
            {
                mb->buffers_next[queue_pos_indices[0]].busy = false;
                shift_queue(mb, 1);
                return first_mb_entry;
            }
        }
        else
        {
            for (uint32_t i = 0; i < mb->num_buffers_in_queue_current; i++)
            {
                struct memory_buffer *mb_entry = &mb->buffers_current[queue_pos_indices[i]];
                if (mb_entry->op == SW || mb_entry->op == SH || mb_entry->op == SB)
                {
                    break;
                }
                if (mb_entry->qj == 0)
                {
                    shift_queue(mb, mb_entry->queue_pos);
                    return mb_entry;
                }
            }
        }
    }
    return NULL;
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

void memory_buffers_clear(struct memory_buffers *mb)
{
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        mb->buffers_next[i].busy = false;
    }
    mb->num_buffers_in_queue_next = 0;
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
