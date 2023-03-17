#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "main_memory.h"
#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"
#include "control.h"

#define NUM_REGS 32

struct cpu
{
    struct main_memory *mm;                // Pointer to main memory
    struct fetch_unit *fetch_unit;         // Pointer to fetch unit
    struct decode_unit *decode_unit;       // Pointer to decode unit
    struct execute_unit *execute_unit;     // Pointer to execute unit
    struct memory_unit *memory_unit;       // Pointer to memory unit
    struct writeback_unit *writeback_unit; // Pointer to writeback unit
    uint32_t regs[NUM_REGS];               // General-purpose registers
    enum ctrl_pc_src ctrl_pc_src;          // Control signal for PC source
    enum ctrl_src_a ctrl_src_a;            // Control signal for source A to ALU
    enum ctrl_src_b ctrl_src_b;            // Control signal for source B to ALU
    enum ctrl_alu_op ctrl_alu_op;          // Control signal for ALU operation
    enum ctrl_cmp ctrl_cmp;                // Control signal for ALU comparison operation
    enum ctrl_mem ctrl_mem;                // Control signal for memory access type
    enum ctrl_wb ctrl_wb;                  // Control signal for writeback source (or lack of)
    uint32_t reg_pc;                       // Program counter
    uint32_t reg_npc;                      // Next program counter
    uint32_t reg_inst;                     // Instruction register
    uint8_t reg_rd_addr;                   // rd address register
    uint32_t reg_rs1_val;                  // rs1 value register
    uint32_t reg_rs2_val;                  // rs2 value register
    uint32_t reg_imm;                      // Immediate register
    uint32_t reg_alu_out;                  // ALU operation output register
    uint32_t reg_mdr;                      // Memory data register
};

struct cpu *cpu_init(char *file_name); // Initialise cpu
void cpu_destroy(struct cpu *cpu);     // Destroy relevent parts of cpu

#endif // CPU_H
