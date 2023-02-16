#ifndef CPU_H
#define CPU_H

#include "memory.h"
#include "fetch.h"
#include "decode.h"
#include "execute.h"

#define NUM_REGS 32

struct cpu
{
    struct memory *memory;             // Memory
    struct fetch_unit *fetch_unit;     // Fetch unit
    struct decode_unit *decode_unit;   // Decode unit
    struct execute_unit *execute_unit; // Execute unit
    uint32_t regs[NUM_REGS];           // General-purpose registers

    uint8_t reg_cond;     // ALU condition output register
    uint32_t reg_alu_out; // ALU operation output register
    uint32_t reg_inst;    // Instruction register
    uint32_t reg_pc;      // Program counter

    uint32_t reg_rs1_val;       // rs1 register
    uint32_t reg_rs2_val;       // rs2 register
    uint32_t reg_imm;           // Immediate register
    uint8_t reg_rd_addr;        // Destination address register
    uint8_t reg_ctrl_alu_op;    // ALU operation control
    uint8_t reg_ctrl_alu_cmp;   // ALU comparison control
    uint8_t reg_ctrl_alu_src_a; // ALU source A control
    uint8_t reg_ctrl_alu_src_b; // ALU source B control

    uint8_t reg_ctrl_cond; // Condition register
};

struct cpu *cpu_init(char *file_name); // Initialise cpu
void cpu_destroy(struct cpu *cpu);     // Destroy relevent parts of cpu

#endif // CPU_H
