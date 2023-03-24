#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "memory_buffers.h"
#include "reg_file.h"
#include "reg.h"

struct memory_buffers *memory_buffers_init(
    uint32_t num_buffers,
    uint32_t id_offset,
    struct reg_file *reg_file,
    struct com_data_bus *cdb,
    struct reg *memory_buffers_all_busy,
    struct reg *reg_buffers_ready)
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
    mb->memory_buffers_ready = reg_buffers_ready;

    for (uint32_t i = 0; i < num_buffers; i++)
    {
        mb->buffers_current[i].id = i + id_offset;
        mb->buffers_current[i].busy = false;
        mb->buffers_next[i].id = i + id_offset;
        mb->buffers_next[i].busy = false;
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
    uint32_t ready = false;
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        all_busy &= mb->buffers_next[i].busy;
        ready |= mb->buffers_next[i].qj == 0 && mb->buffers_next[i].qk == 0 && mb->buffers_next[i].busy;
    }
    reg_write(mb->memory_buffers_all_busy, all_busy);
    reg_write(mb->memory_buffers_ready, ready);
}

void memory_buffers_add(
    struct memory_buffers *mb,
    enum op op,
    uint32_t qj,
    uint32_t qk,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t dest,
    uint32_t inst_pc)
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
            mb->buffers_next[i].inst_pc = inst_pc;
            reg_file_set_reg_qi(mb->reg_file, dest, mb->buffers_next[i].id);
            break;
        }
    }
}

struct memory_buffer memory_buffers_remove(struct memory_buffers *mb)
{
    for (uint32_t i = 0; i < mb->num_buffers; i++)
    {
        if (mb->buffers_current[i].qj == 0 && mb->buffers_current[i].qk == 0 && mb->buffers_current[i].busy)
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

void memory_buffers_update_current(struct memory_buffers *mb)
{
    memcpy(mb->buffers_current, mb->buffers_next, sizeof(struct memory_buffer) * mb->num_buffers);
}

void memory_buffers_destroy(struct memory_buffers *mb)
{
    free(mb->buffers_current);
    free(mb->buffers_next);
    free(mb);
}
