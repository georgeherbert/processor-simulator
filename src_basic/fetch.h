#ifndef FETCH_UNIT_H
#define FETCH_UNIT_H

#include <stdint.h>
#include "main_memory.h"
#include "control.h"

struct fetch_unit
{
    struct main_memory *mm;        // Pointer to main memory
    enum ctrl_pc_src *ctrl_pc_src; // Pointer to control signal for PC source
    uint32_t *reg_alu_out;         // Pointer to ALU output register
    uint32_t *reg_inst;            // Pointer to instruction register
    uint32_t *reg_pc;              // Pointer to program counter register
    uint32_t *reg_npc;             // Pointer to next program counter register
};

struct fetch_unit *fetch_init(
    struct main_memory *mm,
    enum ctrl_pc_src *ctrl_pc_src,
    uint32_t *reg_alu_out,
    uint32_t *reg_inst,
    uint32_t *reg_pc,
    uint32_t *reg_npc);                            // Initialise fetch unit
void fetch_step(struct fetch_unit *fetch_unit);    // Step fetch unit
void fetch_destroy(struct fetch_unit *fetch_unit); // Free fetch unit

#endif // FETCH_UNIT_H
