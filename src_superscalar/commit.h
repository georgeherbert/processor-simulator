#ifndef COMMIT_H
#define COMMIT_H

#include "rob.h"
#include "reg.h"
#include "main_memory.h"
#include "reg_file.h"
#include "res_stations.h"
#include "memory_buffers.h"
#include "alu.h"
#include "inst_queue.h"

struct commit_unit
{
    struct rob *rob;
    struct main_memory *mm;
    struct reg_file *reg_file;
    struct res_stations *alu_rs;
    struct res_stations *branch_rs;
    struct memory_buffers *mb;
    struct alu_unit *alu;
    struct inst_queue *iq;
    struct reg *inst_reg;
    struct reg *pc_src;        // Pointer to control signal for PC source
    struct reg *reg_pc_target; // Pointer to PC target register
    bool *jump_zero;
};

struct commit_unit *commit_init(
    struct rob *rob,
    struct main_memory *mm,
    struct reg_file *reg_file,
    struct res_stations *alu_rs,
    struct res_stations *branch_rs,
    struct memory_buffers *mb,
    struct alu_unit *alu,
    struct inst_queue *iq,
    struct reg *inst_reg,
    struct reg *pc_src,
    struct reg *reg_pc_target,
    bool *jump_zero);
bool commit_step(struct commit_unit *commit_unit);
void commit_destroy(struct commit_unit *commit_unit);

#endif // COMMIT_H
