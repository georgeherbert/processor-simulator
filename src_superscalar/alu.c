#include <stdlib.h>
#include <stdio.h>
#include "alu.h"
#include "res_stations.h"
#include "decoded_inst.h"

struct alu_unit *alu_init(struct res_stations *alu_res_stations, struct reg_file *reg_file)
{
    struct alu_unit *alu_unit = malloc(sizeof(struct alu_unit));

    if (alu_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for alu unit\n");
        exit(EXIT_FAILURE);
    }

    alu_unit->alu_res_stations = alu_res_stations;
    alu_unit->reg_file = reg_file;

    return alu_unit;
}

void alu_step(struct alu_unit *alu_unit)
{
    if (res_stations_is_ready(alu_unit->alu_res_stations))
    {
        struct res_station entry = res_stations_remove(alu_unit->alu_res_stations);

        uint32_t out;

        switch (entry.op)
        {
        case ADD:
        case ADDI:
            out = entry.vj + entry.vk;
            break;
        case LUI:
            out = entry.vj;
            break;
        case AUIPC:
            out = entry.vj + entry.inst_pc;
            break;
        case SUB:
            out = entry.vj - entry.vk;
            break;
        case SLL:
        case SLLI:
            out = entry.vj << entry.vk;
            break;
        case SLT:
        case SLTI:
            out = (int32_t)entry.vj < (int32_t)entry.vk;
            break;
        case SLTU:
        case SLTIU:
            out = entry.vj < entry.vk;
            break;
        case XOR:
        case XORI:
            out = entry.vj ^ entry.vk;
            break;
        case SRL:
        case SRLI:
            out = entry.vj >> entry.vk;
            break;
        case SRA:
        case SRAI:
            out = (int32_t)entry.vj >> entry.vk;
            break;
        case OR:
        case ORI:
            out = entry.vj | entry.vk;
            break;
        case AND:
        case ANDI:
            out = entry.vj & entry.vk;
            break;
        default:
            fprintf(stderr, "Error: Unknown ALU operation\n");
            exit(EXIT_FAILURE);
        }

        // TODO: This should be two steps really...
        if (entry.dest != 0) {
            alu_unit->reg_file->regs[entry.dest].value = out;
        }

        res_stations_set_station_not_busy(alu_unit->alu_res_stations, entry.id);
    }
}

void alu_destroy(struct alu_unit *alu_unit)
{
    free(alu_unit);
}
