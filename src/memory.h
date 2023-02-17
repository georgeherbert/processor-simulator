#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include "main_memory.h"
#include "control.h"

struct memory_unit
{
    struct main_memory *mm;  // Pointer to main memory
    enum ctrl_mem *ctrl_mem; // Pointer to control signal for memory access type
    uint32_t *reg_alu_out;   // Pointer to ALU operation output register
    uint32_t *reg_rs2_val;   // Pointer to rs2 value register
    uint32_t *reg_mdr;       // Pointer to Memory data register
};

struct memory_unit *memory_init(
    struct main_memory *mm,
    enum ctrl_mem *ctrl_mem,
    uint32_t *reg_alu_out,
    uint32_t *reg_rs2_val,
    uint32_t *reg_mdr);                            // Initialise memory unit
void memory_step(struct memory_unit *memory_unit); // Step memory unit
void memory_destroy(struct memory_unit *memory_unit);

#endif // MEMORY_H
