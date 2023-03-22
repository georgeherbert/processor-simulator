#ifndef FETCH_UNIT_H
#define FETCH_UNIT_H

#include <stdint.h>
#include <stdbool.h>
#include "main_memory.h"
#include "control.h"
#include "inst_queue.h"
#include "reg.h"

struct fetch_unit
{
    struct main_memory *mm;         // Pointer to main memory
    struct reg *pc_src;             // Pointer to control signal for PC source
    struct reg *branch_in_pipeline; // Pointer to control signal for presence of branch in pipeline
    struct inst_queue *inst_queue;  // Pointer to instruction queue
    struct reg *reg_pc_target;      // Pointer to PC target register
    struct reg *reg_inst;           // Pointer to instruction register
    struct reg *reg_inst_pc;        // Pointer to program counter of instruction in instruction register
    uint32_t reg_pc;                // Program counter register
    uint32_t reg_npc;               // Next program counter register
};

struct fetch_unit *fetch_init(
    struct main_memory *mm,
    struct reg *pc_src,
    struct reg *branch_in_pipeline,
    struct inst_queue *inst_queue,
    struct reg *reg_pc_target,
    struct reg *reg_inst,
    struct reg *reg_inst_pc);                      // Initialise fetch unit
void fetch_step(struct fetch_unit *fetch_unit);    // Step fetch unit
void fetch_destroy(struct fetch_unit *fetch_unit); // Free fetch unit

#endif // FETCH_UNIT_H
