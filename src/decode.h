#ifndef DECODE_H
#define DECODE_H

#include "memory.h"

struct decode_unit
{
    struct memory *memory; // Pointer to memory
    uint32_t *reg_inst;    // Pointer to instruction register
    uint32_t *reg_rs1;     // Pointer to source register 1
    uint32_t *reg_rs2;     // Pointer to source register 2
    uint32_t *reg_imm;     // Pointer to immediate register
};

struct decode_unit *decode_init(struct memory *memory, uint32_t *reg_inst, uint32_t *reg_rs1, uint32_t *reg_rs2, uint32_t *reg_imm); // Initialise decode unit
void decode_step(struct decode_unit *decode_unit);                                                                                   // Step decode unit
void decode_destroy(struct decode_unit *decode_unit);                                                                                // Free decode unit

#endif // DECODE_H
