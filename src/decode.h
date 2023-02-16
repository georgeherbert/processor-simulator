#ifndef DECODE_H
#define DECODE_H

#include "memory.h"

struct decode_unit
{
    uint32_t *regs;        // Pointer to general-purpose registers
    uint32_t *reg_inst;    // Pointer to instruction register
    uint32_t *reg_rs1_val; // Pointer to source register 1
    uint32_t *reg_rs2_val; // Pointer to source register 2
    uint32_t *reg_imm;     // Pointer to immediate register

    uint8_t *reg_rd_addr;        // Pointer tot destination address register
    uint8_t *reg_ctrl_alu_op;    // Pointer to ALU operation control
    uint8_t *reg_ctrl_alu_cmp;   // Pointer to ALU comparison control
    uint8_t *reg_ctrl_alu_src_a; // Pointer to ALU source A control
    uint8_t *reg_ctrl_alu_src_b; // Pointer to ALU source B control
};

struct decode_unit *decode_init(
    uint32_t *regs,
    uint32_t *reg_inst,
    uint32_t *reg_rs1_val,
    uint32_t *reg_rs2_val,
    uint32_t *reg_imm,
    uint8_t *reg_rd_addr,
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b);                     // Initialise decode unit
void decode_step(struct decode_unit *decode_unit);    // Step decode unit
void decode_destroy(struct decode_unit *decode_unit); // Free decode unit

#endif // DECODE_H
