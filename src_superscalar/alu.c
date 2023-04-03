#include <stdlib.h>
#include <stdio.h>
#include "alu.h"
#include "res_stations.h"
#include "decoded_inst.h"
#include "cdb.h"
#include "reg.h"

struct alu_unit *alu_init(
    uint8_t id,
    struct res_stations *alu_res_stations,
    struct reg_file *reg_file,
    struct cdb *cdb)
{
    struct alu_unit *alu_unit = malloc(sizeof(struct alu_unit));

    if (alu_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for alu unit\n");
        exit(EXIT_FAILURE);
    }

    alu_unit->id = id;
    alu_unit->alu_res_stations = alu_res_stations;
    alu_unit->reg_file = reg_file;
    alu_unit->cdb = cdb;

    alu_unit->num_cycles = 1;
    alu_unit->relative_cycle = 0;

    return alu_unit;
}

void alu_step(struct alu_unit *alu_unit)
{
    if (alu_unit->relative_cycle == 0)
    {
        struct res_station *rs_entry = res_stations_remove(alu_unit->alu_res_stations, alu_unit->id);
        if (rs_entry)
        {
            alu_unit->relative_cycle++;

            uint32_t out;

            switch (rs_entry->op)
            {
            case ADD:
            case ADDI:
                out = rs_entry->vj + rs_entry->vk;
                break;
            case LUI:
                out = rs_entry->vj;
                break;
            case AUIPC:
                out = rs_entry->vj + rs_entry->inst_pc;
                break;
            case SUB:
                out = rs_entry->vj - rs_entry->vk;
                break;
            case SLL:
            case SLLI:
                out = rs_entry->vj << rs_entry->vk;
                break;
            case SLT:
            case SLTI:
                out = (int32_t)rs_entry->vj < (int32_t)rs_entry->vk;
                break;
            case SLTU:
            case SLTIU:
                out = rs_entry->vj < rs_entry->vk;
                break;
            case XOR:
            case XORI:
                out = rs_entry->vj ^ rs_entry->vk;
                break;
            case SRL:
            case SRLI:
                out = rs_entry->vj >> rs_entry->vk;
                break;
            case SRA:
            case SRAI:
                out = (int32_t)rs_entry->vj >> rs_entry->vk;
                break;
            case OR:
            case ORI:
                out = rs_entry->vj | rs_entry->vk;
                break;
            case AND:
            case ANDI:
                out = rs_entry->vj & rs_entry->vk;
                break;
            default:
                fprintf(stderr, "Error: Unknown ALU operation\n");
                exit(EXIT_FAILURE);
            }

            alu_unit->entry_rob_id = rs_entry->rob_id;
            alu_unit->out = out;
        }
    }
    else if (alu_unit->relative_cycle > 0 && alu_unit->relative_cycle < alu_unit->num_cycles)
    {
        alu_unit->relative_cycle++;
    }
    else if (alu_unit->relative_cycle == alu_unit->num_cycles)
    {
        cdb_write(alu_unit->cdb, alu_unit->entry_rob_id, alu_unit->out);
        alu_unit->relative_cycle = 0;
    }
}

void alu_clear(struct alu_unit *alu_unit)
{
    alu_unit->relative_cycle = 0;
}

void alu_destroy(struct alu_unit *alu_unit)
{
    free(alu_unit);
}
