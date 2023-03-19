#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "branch.h"
#include "res_stations.h"
#include "reg_file.h"
#include "control.h"

struct branch_unit *branch_init(
    struct res_stations *branch_res_stations,
    struct reg_file *reg_file,
    uint32_t *reg_pc_target,
    enum pc_src *pc_src)
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

    return branch_unit;
}

void branch_step(struct branch_unit *branch_unit)
{
    if (res_stations_is_ready(branch_unit->branch_res_stations))
    {
        struct res_station entry = res_stations_remove(branch_unit->branch_res_stations);

        switch (entry.op)
        {
        case JAL:
            *branch_unit->pc_src = PC_SRC_BRANCH;
            *branch_unit->reg_pc_target = entry.a + entry.inst_pc;
            if (entry.dest != 0)
            {
                branch_unit->reg_file->regs[entry.dest].value = entry.inst_pc + 4;
            }
            break;
        case JALR:
            *branch_unit->pc_src = PC_SRC_BRANCH;
            *branch_unit->reg_pc_target = (entry.vj + entry.a) & ~1;
            if (entry.dest != 0)
            {
                branch_unit->reg_file->regs[entry.dest].value = entry.inst_pc + 4;
            }
            break;
        case BEQ:
            *branch_unit->pc_src = entry.vj == entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4;
            *branch_unit->reg_pc_target = (entry.a + entry.inst_pc);
            break;
        case BNE:
            *branch_unit->pc_src = entry.vj != entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4;
            *branch_unit->reg_pc_target = (entry.a + entry.inst_pc);
            break;
        case BLT:
            *branch_unit->pc_src = (int32_t)entry.vj < (int32_t)entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4;
            *branch_unit->reg_pc_target = (entry.a + entry.inst_pc);
            break;
        case BLTU:
            *branch_unit->pc_src = entry.vj < entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4;
            *branch_unit->reg_pc_target = (entry.a + entry.inst_pc);
            break;
        case BGE:
            *branch_unit->pc_src = (int32_t)entry.vj >= (int32_t)entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4;
            *branch_unit->reg_pc_target = (entry.a + entry.inst_pc);
            break;
        case BGEU:
            *branch_unit->pc_src = entry.vj >= entry.vk ? PC_SRC_BRANCH : PC_SRC_PLUS_4;
            *branch_unit->reg_pc_target = (entry.a + entry.inst_pc);
            break;
        default:
            fprintf(stderr, "Error: Unknown branch or jump operation\n");
            exit(EXIT_FAILURE);
        }

        res_stations_set_station_not_busy(branch_unit->branch_res_stations, entry.id);
    }
}

void branch_destroy(struct branch_unit *branch_unit)
{
    free(branch_unit);
}
