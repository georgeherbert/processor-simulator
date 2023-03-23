#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "res_stations.h"
#include "reg_file.h"
#include "main_memory.h"
#include "reg.h"

struct memory_unit *memory_init(
    struct res_stations *memory_res_stations,
    struct main_memory *mm,
    struct reg_file *reg_file,
    struct com_data_bus *cdb,
    struct reg *res_stations_ready_memory)
{
    struct memory_unit *memory_unit = malloc(sizeof(struct memory_unit));

    if (memory_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for memory unit\n");
        exit(EXIT_FAILURE);
    }

    memory_unit->memory_res_stations = memory_res_stations;
    memory_unit->mm = mm;
    memory_unit->reg_file = reg_file;
    memory_unit->cdb = cdb;
    memory_unit->res_stations_ready_memory = res_stations_ready_memory;

    return memory_unit;
}

void memory_step(struct memory_unit *memory_unit)
{
    if (reg_read(memory_unit->res_stations_ready_memory))
    {
        struct res_station entry = res_stations_remove(memory_unit->memory_res_stations);

        uint32_t address = entry.vj + entry.a;

        switch (entry.op)
        {
        case LW:
            com_data_bus_add_entry(memory_unit->cdb, entry.id, main_memory_load_word(memory_unit->mm, address));
            break;
        case LH:
            com_data_bus_add_entry(memory_unit->cdb, entry.id, (int32_t)(int16_t)main_memory_load_half(memory_unit->mm, address));
            break;
        case LHU:
            com_data_bus_add_entry(memory_unit->cdb, entry.id, main_memory_load_half(memory_unit->mm, address));
            break;
        case LB:
            com_data_bus_add_entry(memory_unit->cdb, entry.id, (int32_t)(int8_t)main_memory_load_byte(memory_unit->mm, address));
            break;
        case LBU:
            com_data_bus_add_entry(memory_unit->cdb, entry.id, main_memory_load_byte(memory_unit->mm, address));
            break;
        case SW:
            main_memory_store_word(memory_unit->mm, address, entry.vk);
            break;
        case SH:
            main_memory_store_half(memory_unit->mm, address, entry.vk);
            break;
        case SB:
            main_memory_store_byte(memory_unit->mm, address, entry.vk);
            break;
        default:
            fprintf(stderr, "Error: Unknown memory operation");
            exit(EXIT_FAILURE);
        }

        res_stations_set_station_not_busy(memory_unit->memory_res_stations, entry.id);
    }
}

void memory_destroy(struct memory_unit *memory_unit)
{
    free(memory_unit);
}
