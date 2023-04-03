#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "reg_file.h"
#include "main_memory.h"
#include "reg.h"
#include "memory_buffers.h"

struct memory_unit *memory_init(
    uint8_t id,
    struct memory_buffers *memory_buffers,
    struct main_memory *mm,
    struct reg_file *reg_file,
    struct cdb *cdb)
{
    struct memory_unit *mu = malloc(sizeof(struct memory_unit));

    if (mu == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for memory unit\n");
        exit(EXIT_FAILURE);
    }

    mu->id = id;
    mu->memory_buffers = memory_buffers;
    mu->mm = mm;
    mu->reg_file = reg_file;
    mu->cdb = cdb;

    mu->num_cycles = 1;
    mu->relative_cycle = 0;

    return mu;
}

void memory_step(struct memory_unit *mu)
{
    if (mu->relative_cycle == 0)
    {
        struct memory_buffer *mb_entry = memory_buffers_dequeue_memory(mu->memory_buffers, mu->id);

        if (mb_entry)
        {
            mu->relative_cycle++;

            switch (mb_entry->op)
            {
            case LW:
                mu->out = main_memory_load_word(mu->mm, mb_entry->a);
                break;
            case LH:
                mu->out = (int32_t)(int16_t)main_memory_load_half(mu->mm, mb_entry->a);
                break;
            case LHU:
                mu->out = main_memory_load_half(mu->mm, mb_entry->a);
                break;
            case LB:
                mu->out = (int32_t)(int8_t)main_memory_load_byte(mu->mm, mb_entry->a);
                break;
            case LBU:
                mu->out = main_memory_load_byte(mu->mm, mb_entry->a);
                break;
            default:
                fprintf(stderr, "Error: Unknown memory operation");
                exit(EXIT_FAILURE);
            }

            mu->entry_rob_id = mb_entry->rob_id;
        }
    }
    else if (mu->relative_cycle > 0 && mu->relative_cycle < mu->num_cycles)
    {
        mu->relative_cycle++;
    }
    else if (mu->relative_cycle == mu->num_cycles)
    {
        cdb_write(mu->cdb, mu->entry_rob_id, mu->out);
        mu->relative_cycle = 0;
    }
}

void memory_clear(struct memory_unit *mu)
{
    mu->relative_cycle = 0;
}

void memory_destroy(struct memory_unit *mu)
{
    free(mu);
}
