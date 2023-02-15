#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include "fetch.h"
#include "decode.h"
#include "execute.h"

#define NUM_REGS 32

struct cpu
{
    struct memory memory;             // Memory
    struct fetch_unit fetch_unit;     // Fetch unit
    struct decode_unit decode_unit;   // Decode unit
    struct execute_unit execute_unit; // Execute unit
    uint32_t regs[NUM_REGS];          // General-purpose registers
    uint32_t reg_pc;                  // Program counter
    uint32_t reg_npc;                 // Next program counter
    uint32_t reg_inst;                // Instruction register
    uint32_t reg_rs1;                 // Source register 1
    uint32_t reg_rs2;                 // Source register 2
    uint32_t reg_imm;                 // Immediate register
};

struct cpu cpu_init(char *file_name); // Initialise cpu
void cpu_destroy(struct cpu *cpu);    // Destroy relevent parts of cpu

#endif // CPU_H
