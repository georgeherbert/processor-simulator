#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "branch.h"
#include "res_stations.h"
#include "reg_file.h"
#include "control.h"
#include "cdb.h"
#include "reg.h"

struct branch_unit *branch_init(
    struct res_stations *branch_res_stations,
    struct reg_file *reg_file,
    struct cdb *cdb,
    struct rob *rob)
{
    struct branch_unit *branch_unit = malloc(sizeof(struct branch_unit));

    if (branch_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for branch unit\n");
        exit(EXIT_FAILURE);
    }

    branch_unit->branch_res_stations = branch_res_stations;
    branch_unit->reg_file = reg_file;
    branch_unit->cdb = cdb;
    branch_unit->rob = rob;

    return branch_unit;
}

void branch_step(struct branch_unit *branch_unit)
{
    struct res_station *rs_entry = res_stations_remove(branch_unit->branch_res_stations);

    uint32_t npc_actual;

    if (rs_entry)
    {
        switch (rs_entry->op)
        {
        case JAL:
            npc_actual = rs_entry->a + rs_entry->inst_pc;
            cdb_write(branch_unit->cdb, rs_entry->rob_id, rs_entry->inst_pc + 4);
            break;
        case JALR:
            npc_actual = (rs_entry->vj + rs_entry->a) & ~1;
            cdb_write(branch_unit->cdb, rs_entry->rob_id, rs_entry->inst_pc + 4);
            break;
        case BEQ:
            npc_actual = rs_entry->vj == rs_entry->vk ? (rs_entry->a + rs_entry->inst_pc) : rs_entry->inst_pc + 4;
            break;
        case BNE:
            npc_actual = rs_entry->vj != rs_entry->vk ? (rs_entry->a + rs_entry->inst_pc) : rs_entry->inst_pc + 4;
            break;
        case BLT:
            npc_actual = (int32_t)rs_entry->vj < (int32_t)rs_entry->vk ? (rs_entry->a + rs_entry->inst_pc) : rs_entry->inst_pc + 4;
            break;
        case BLTU:
            npc_actual = rs_entry->vj < rs_entry->vk ? (rs_entry->a + rs_entry->inst_pc) : rs_entry->inst_pc + 4;
            break;
        case BGE:
            npc_actual = (int32_t)rs_entry->vj >= (int32_t)rs_entry->vk ? (rs_entry->a + rs_entry->inst_pc) : rs_entry->inst_pc + 4;
            break;
        case BGEU:
            npc_actual = rs_entry->vj >= rs_entry->vk ? (rs_entry->a + rs_entry->inst_pc) : rs_entry->inst_pc + 4;
            break;
        default:
            fprintf(stderr, "Error: Unknown branch or jump operation\n");
            exit(EXIT_FAILURE);
        }

        rob_add_npc_actual(branch_unit->rob, rs_entry->rob_id, npc_actual);
    }
}

void branch_destroy(struct branch_unit *branch_unit)
{
    free(branch_unit);
}
