#include <stdlib.h>
#include <stdio.h>
#include "alu.h"
#include "res_stations.h"
#include "decoded_inst.h"
#include "com_data_bus.h"

struct alu_unit *alu_init(struct res_stations *alu_res_stations, struct reg_file *reg_file, struct com_data_bus *cdb)
{
    struct alu_unit *alu_unit = malloc(sizeof(struct alu_unit));

    if (alu_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for alu unit\n");
        exit(EXIT_FAILURE);
    }

    alu_unit->alu_res_stations = alu_res_stations;
    alu_unit->reg_file = reg_file;
    alu_unit->cdb = cdb;

    alu_unit->num_cycles = 10;
    alu_unit->relative_cycle = 0;

    return alu_unit;
}

void alu_step(struct alu_unit *alu_unit)
{
    if (alu_unit->relative_cycle == 0 && res_stations_is_ready(alu_unit->alu_res_stations))
    {
        alu_unit->relative_cycle++;

        struct res_station entry = res_stations_remove(alu_unit->alu_res_stations);

        uint32_t out;

        switch (entry.op)
        {
        case ADD:
        case ADDI:
            // printf("ADD(I)\n");
            out = entry.vj + entry.vk;
            break;
        case LUI:
            // printf("LUI\n");
            out = entry.vj;
            break;
        case AUIPC:
            // printf("AUIPC\n");
            out = entry.vj + entry.inst_pc;
            break;
        case SUB:
            // printf("SUB\n");
            out = entry.vj - entry.vk;
            break;
        case SLL:
        case SLLI:
            // printf("SLL(I)\n");
            out = entry.vj << entry.vk;
            break;
        case SLT:
        case SLTI:
            // printf("SLT(I)\n");
            out = (int32_t)entry.vj < (int32_t)entry.vk;
            break;
        case SLTU:
        case SLTIU:
            // printf("SLTU(I)\n");
            out = entry.vj < entry.vk;
            break;
        case XOR:
        case XORI:
            // printf("XOR(I)\n");
            out = entry.vj ^ entry.vk;
            break;
        case SRL:
        case SRLI:
            // printf("SRL(I)\n");
            out = entry.vj >> entry.vk;
            break;
        case SRA:
        case SRAI:
            // printf("SRA(I)\n");
            out = (int32_t)entry.vj >> entry.vk;
            break;
        case OR:
        case ORI:
            // printf("OR(I)\n");
            out = entry.vj | entry.vk;
            break;
        case AND:
        case ANDI:
            // printf("AND(I)\n");
            out = entry.vj & entry.vk;
            break;
        default:
            fprintf(stderr, "Error: Unknown ALU operation\n");
            exit(EXIT_FAILURE);
        }

        alu_unit->entry_id = entry.id;
        alu_unit->out = out;

        // TODO: This should be two steps really...
        // com_data_bus_add_entry(alu_unit->cdb, entry.id, out);

        // res_stations_set_station_not_busy(alu_unit->alu_res_stations, entry.id);
    }
    else if (alu_unit->relative_cycle > 0 && alu_unit->relative_cycle < alu_unit->num_cycles)
    {
        alu_unit->relative_cycle++;
    }
    else if (alu_unit->relative_cycle == alu_unit->num_cycles)
    {
        com_data_bus_add_entry(alu_unit->cdb, alu_unit->entry_id, alu_unit->out);
        res_stations_set_station_not_busy(alu_unit->alu_res_stations, alu_unit->entry_id);
        alu_unit->relative_cycle = 0;
    }
}

void alu_destroy(struct alu_unit *alu_unit)
{
    free(alu_unit);
}
