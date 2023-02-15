#ifndef FETCH_UNIT_H
#define FETCH_UNIT_H

#include "memory.h"

struct fetch_unit
{
    struct memory *memory; // Pointer to memory
    uint32_t *reg_pc;      // Pointer to program counter
    uint32_t *reg_npc;     // Pointer to next program counter
    uint32_t *reg_inst;    // Pointer to instruction register
};

struct fetch_unit fetch_init(struct memory *memory, uint32_t *reg_pc, uint32_t *reg_npc, uint32_t *reg_inst); // Initialise fetch unit
void fetch_step(struct fetch_unit *fetch_unit);                                                               // Step fetch unit

#endif // FETCH_UNIT_H
