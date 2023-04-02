#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "reg_file.h"
#include "main_memory.h"
#include "reg.h"
#include "memory_buffers.h"

struct memory_unit *memory_init(
    struct memory_buffers *memory_buffers,
    struct main_memory *mm,
    struct reg_file *reg_file,
    struct cdb *cdb)
{
    struct memory_unit *memory_unit = malloc(sizeof(struct memory_unit));

    if (memory_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for memory unit\n");
        exit(EXIT_FAILURE);
    }

    memory_unit->memory_buffers = memory_buffers;
    memory_unit->mm = mm;
    memory_unit->reg_file = reg_file;
    memory_unit->cdb = cdb;

    return memory_unit;
}

void memory_step(struct memory_unit *memory_unit)
{
    struct memory_buffer *mb_entry = memory_buffers_dequeue_memory(memory_unit->memory_buffers);

    if (mb_entry)
    {
        switch (mb_entry->op)
        {
        case LW:
            cdb_write(memory_unit->cdb, mb_entry->rob_id, main_memory_load_word(memory_unit->mm, mb_entry->a));
            break;
        case LH:
            cdb_write(memory_unit->cdb, mb_entry->rob_id, (int32_t)(int16_t)main_memory_load_half(memory_unit->mm, mb_entry->a));
            break;
        case LHU:
            cdb_write(memory_unit->cdb, mb_entry->rob_id, main_memory_load_half(memory_unit->mm, mb_entry->a));
            break;
        case LB:
            cdb_write(memory_unit->cdb, mb_entry->rob_id, (int32_t)(int8_t)main_memory_load_byte(memory_unit->mm, mb_entry->a));
            break;
        case LBU:
            cdb_write(memory_unit->cdb, mb_entry->rob_id, main_memory_load_byte(memory_unit->mm, mb_entry->a));
            break;
        default:
            fprintf(stderr, "Error: Unknown memory operation");
            exit(EXIT_FAILURE);
        }
    }
}

void memory_destroy(struct memory_unit *memory_unit)
{
    free(memory_unit);
}
