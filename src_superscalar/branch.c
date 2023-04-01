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
    struct reg *reg_pc_target,
    struct reg *pc_src,
    struct cdb *cdb,
    struct reg *branch_in_pipeline,
    struct reg *res_stations_ready_branch)
{
    struct branch_unit *branch_unit = malloc(sizeof(struct branch_unit));

    if (branch_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for branch unit\n");
        exit(EXIT_FAILURE);
    }

    branch_unit->branch_res_stations = branch_res_stations;
    branch_unit->reg_file = reg_file;
    branch_unit->reg_pc_target = reg_pc_target;
    branch_unit->pc_src = pc_src;
    branch_unit->cdb = cdb;
    branch_unit->branch_in_pipeline = branch_in_pipeline;
    branch_unit->res_stations_ready_branch = res_stations_ready_branch;

    return branch_unit;
}

void branch_step(struct branch_unit *branch_unit)
{
    if (reg_read(branch_unit->res_stations_ready_branch))
    {
        struct res_station entry = res_stations_remove(branch_unit->branch_res_stations);

        switch (entry.op)
        {
        case JAL:
            reg_write(branch_unit->pc_src, PC_SRC_BRANCH);
            reg_write(branch_unit->reg_pc_target, entry.a + entry.inst_pc);
            cdb_write(branch_unit->cdb, entry.rob_id, entry.inst_pc + 4);
            break;
        case JALR:
            reg_write(branch_unit->pc_src, PC_SRC_BRANCH);
            reg_write(branch_unit->reg_pc_target, (entry.vj + entry.a) & ~1);
            cdb_write(branch_unit->cdb, entry.rob_id, entry.inst_pc + 4);
            break;
        case BEQ:
            reg_write(branch_unit->pc_src, entry.vj == entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4);
            reg_write(branch_unit->reg_pc_target, (entry.a + entry.inst_pc));
            cdb_write(branch_unit->cdb, entry.rob_id, entry.vj == entry.vk);
            break;
        case BNE:
            reg_write(branch_unit->pc_src, entry.vj != entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4);
            reg_write(branch_unit->reg_pc_target, (entry.a + entry.inst_pc));
            cdb_write(branch_unit->cdb, entry.rob_id, entry.vj != entry.vk);
            break;
        case BLT:
            reg_write(branch_unit->pc_src, (int32_t)entry.vj < (int32_t)entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4);
            reg_write(branch_unit->reg_pc_target, (entry.a + entry.inst_pc));
            cdb_write(branch_unit->cdb, entry.rob_id, (int32_t)entry.vj < (int32_t)entry.vk);
            break;
        case BLTU:
            reg_write(branch_unit->pc_src, entry.vj < entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4);
            reg_write(branch_unit->reg_pc_target, (entry.a + entry.inst_pc));
            cdb_write(branch_unit->cdb, entry.rob_id, entry.vj < entry.vk);
            break;
        case BGE:
            reg_write(branch_unit->pc_src, (int32_t)entry.vj >= (int32_t)entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4);
            reg_write(branch_unit->reg_pc_target, (entry.a + entry.inst_pc));
            cdb_write(branch_unit->cdb, entry.rob_id, (int32_t)entry.vj >= (int32_t)entry.vk);
            break;
        case BGEU:
            reg_write(branch_unit->pc_src, entry.vj >= entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4);
            reg_write(branch_unit->reg_pc_target, (entry.a + entry.inst_pc));
            cdb_write(branch_unit->cdb, entry.rob_id, entry.vj >= entry.vk);
            break;
        default:
            fprintf(stderr, "Error: Unknown branch or jump operation\n");
            exit(EXIT_FAILURE);
        }

        reg_write(branch_unit->branch_in_pipeline, BRANCH_NOT_IN_PIPELINE);
        res_stations_set_station_not_busy(branch_unit->branch_res_stations, entry.id);
    }
}

void branch_destroy(struct branch_unit *branch_unit)
{
    free(branch_unit);
}
