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
    struct com_data_bus *cdb,
    struct reg *memory_buffers_ready)
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
    memory_unit->memory_buffers_ready = memory_buffers_ready;

    return memory_unit;
}

void memory_step(struct memory_unit *memory_unit)
{
    if (reg_read(memory_unit->memory_buffers_ready))
    {
        struct memory_buffer entry = *memory_buffers_remove(memory_unit->memory_buffers);

        switch (entry.op)
        {
        case LW:
            com_data_bus_add_entry(memory_unit->cdb, entry.rob_id, main_memory_load_word(memory_unit->mm, entry.a));
            break;
        case LH:
            com_data_bus_add_entry(memory_unit->cdb, entry.rob_id, (int32_t)(int16_t)main_memory_load_half(memory_unit->mm, entry.a));
            break;
        case LHU:
            com_data_bus_add_entry(memory_unit->cdb, entry.rob_id, main_memory_load_half(memory_unit->mm, entry.a));
            break;
        case LB:
            com_data_bus_add_entry(memory_unit->cdb, entry.rob_id, (int32_t)(int8_t)main_memory_load_byte(memory_unit->mm, entry.a));
            break;
        case LBU:
            com_data_bus_add_entry(memory_unit->cdb, entry.rob_id, main_memory_load_byte(memory_unit->mm, entry.a));
            break;
        default:
            fprintf(stderr, "Error: Unknown memory operation");
            exit(EXIT_FAILURE);
        }

        memory_buffers_set_buffer_not_busy(memory_unit->memory_buffers, entry.id);
    }
}

void memory_destroy(struct memory_unit *memory_unit)
{
    free(memory_unit);
}
