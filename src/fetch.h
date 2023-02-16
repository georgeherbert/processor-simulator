#ifndef FETCH_UNIT_H
#define FETCH_UNIT_H

#include "memory.h"

struct fetch_unit
{
    struct memory *memory; // Pointer to memory
    uint8_t *reg_cond;     // Pointer to condition register
    uint32_t *reg_alu_out; // Pointer to ALU output register
    uint32_t *reg_inst;    // Pointer to instruction register
    uint32_t *reg_pc;      // Pointer to program counter register
};

struct fetch_unit *fetch_init(
    struct memory *memory,
    uint8_t *reg_cond,
    uint32_t *reg_alu_out,
    uint32_t *reg_inst,
    uint32_t *reg_pc);                             // Initialise fetch unit
void fetch_step(struct fetch_unit *fetch_unit);    // Step fetch unit
void fetch_destroy(struct fetch_unit *fetch_unit); // Free fetch unit

#endif // FETCH_UNIT_H
