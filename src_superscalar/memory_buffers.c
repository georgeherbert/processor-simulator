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
    struct com_data_bus *cdb,
    struct reg *memory_buffers_all_busy,
    struct reg *memory_buffers_ready_address,
    struct reg *memory_buffers_ready_memory)
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
                if (com_data_bus_is_val_ready(mb->cdb, mb->buffers_next[i].qj))
                {
                    mb->buffers_next[i].vj = com_data_bus_get_val(mb->cdb, mb->buffers_next[i].qj);
                    mb->buffers_next[i].qj = 0;
                }
            }
            if (mb->buffers_next[i].qk != 0)
            {
                if (com_data_bus_is_val_ready(mb->cdb, mb->buffers_next[i].qk))
                {
                    mb->buffers_next[i].vk = com_data_bus_get_val(mb->cdb, mb->buffers_next[i].qk);
                    mb->buffers_next[i].qk = 0;
                }
            }
        }
    }

    bool all_busy = true;
    uint32_t ready_address = false;
    uint32_t ready_memory = false;
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        all_busy &= mb->buffers_next[i].busy;
        ready_address |= mb->buffers_next[i].busy && mb->buffers_next[i].qj == 0 && mb->buffers_next[i].queue_pos == 1;
        ready_memory |= mb->buffers_next[i].busy && mb->buffers_next[i].qk == 0 && mb->buffers_next[i].queue_pos == 0;
        /*
            The queue_pos of zero above indicates the entry has already had its address computed via the address unit.
            As such, it is ready for the memory unit (assuming the other conditions are also true).
        */
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
    uint32_t dest)
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
            /*
                The +1 in the next line ensures that the queue position is never 0.
                This is because 0 is used to indicate that the buffer is not in the queue.
            */
            mb->buffers_next[i].queue_pos = mb->num_buffers_in_queue_current + 1;
            reg_file_set_reg_qi(mb->reg_file, dest, mb->buffers_next[i].id);
            mb->num_buffers_in_queue_next++;
            break;
        }
    }
}

struct memory_buffer memory_buffers_remove(struct memory_buffers *mb)
{
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (mb->buffers_current[i].busy && mb->buffers_current[i].qk == 0 && mb->buffers_current[i].queue_pos == 0)
        {
            return mb->buffers_current[i];
        }
    }
    fprintf(stderr, "Error: Cannot find a ready memory buffer despite checking if there is one\n");
    exit(EXIT_FAILURE);
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

struct memory_buffer memory_buffers_dequeue(struct memory_buffers *mb)
{
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (mb->buffers_current[i].busy && mb->buffers_current[i].qj == 0 && mb->buffers_current[i].queue_pos == 1)
        {
            return mb->buffers_current[i];
        }
    }
    fprintf(stderr, "Error: Cannot find a ready memory buffer despite checking if there is one\n");
    exit(EXIT_FAILURE);
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
    enum op op = mb->buffers_next[id_index].op;
    bool is_load = op == LW || op == LH || op == LHU || op == LB || op == LBU;
    bool is_store = op == SW || op == SH || op == SB;
    /*
        We define an "address conflict" as follows. For a load instruction, we cannot add the effective address
        to the memory buffer if there is a corresponding store in the memory buffer with the same effective address.
        For a store instruction, we cannot add the effective address to the memory buffer if there is a corresponding
        load OR store in the memory buffer with the same effective address.
    */
    bool address_conflict = false;
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (mb->buffers_next[i].busy && mb->buffers_next[i].queue_pos == 0 && mb->buffers_next[i].a == address)
        {
            address_conflict = is_store;
            address_conflict |= is_load && (mb->buffers_next[i].op == SW || mb->buffers_next[i].op == SH || mb->buffers_next[i].op == SB);
        }
    }
    if (!address_conflict)
    {
        mb->buffers_next[id_index].a = address;
        mb->buffers_next[id_index].queue_pos = 0;
        mb->num_buffers_in_queue_next--;
        for (uint32_t i = 0; i < mb->num_buffers; i++)
        {
            if (mb->buffers_next[i].busy && mb->buffers_next[i].queue_pos > 0)
            {
                mb->buffers_next[i].queue_pos--;
            }
        }
    }
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
