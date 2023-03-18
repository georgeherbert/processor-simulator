#include <stdlib.h>
#include <stdio.h>
#include "alu.h"
#include "res_station.h"
#include "decoded_inst.h"

struct alu_unit *alu_init(struct res_station *alu_res_station, uint32_t *regs)
{
    struct alu_unit *alu_unit = malloc(sizeof(struct alu_unit));

    if (alu_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for alu unit\n");
        exit(EXIT_FAILURE);
    }

    alu_unit->alu_res_station = alu_res_station;
    alu_unit->regs = regs;

    return alu_unit;
}

void alu_step(struct alu_unit *alu_unit)
{
    if (res_station_get_num_entries(alu_unit->alu_res_station) != 0)
    {
        struct res_station_entry entry = res_station_remove(alu_unit->alu_res_station);

        uint32_t out;

        switch (entry.op)
        {
        case ADD:
        case ADDI:
        case LUI:
        case AUIPC:
            out = entry.vj + entry.vk;
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
            alu_unit->regs[entry.dest] = out;
        }
    }
}

void alu_destroy(struct alu_unit *alu_unit)
{
    free(alu_unit);
}
