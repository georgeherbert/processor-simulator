#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>
#include "control.h"

struct decode_unit
{
    uint32_t *regs; // Pointer to general-purpose registers

    uint32_t *reg_inst;            // Pointer to instruction register
    uint8_t *reg_rd_addr;          // Pointer tot destination address register
    uint32_t *reg_rs1_val;         // Pointer to source register 1
    uint32_t *reg_rs2_val;         // Pointer to source register 2
    uint32_t *reg_imm;             // Pointer to immediate register
    enum ctrl_src_a *ctrl_src_a;   // Pointer to control signal for source A to ALU
    enum ctrl_src_b *ctrl_src_b;   // Pointer to control signal for source B to ALU
    enum ctrl_alu_op *ctrl_alu_op; // Pointer to control signal for ALU operation
    enum ctrl_cmp *ctrl_cmp;       // Pointer to control signal for ALU comparison operation
    enum ctrl_mem *ctrl_mem;       // Pointer to control signal for memory access type
    enum ctrl_wb *ctrl_wb;         // Pointer to control signal for writeback source (or lack of)
};

struct decode_unit *decode_init(
    uint32_t *regs,
    uint32_t *reg_inst,
    uint8_t *reg_rd_addr,
    uint32_t *reg_rs1_val,
    uint32_t *reg_rs2_val,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb);                           // Initialise decode unit
void decode_step(struct decode_unit *decode_unit);    // Step decode unit
void decode_destroy(struct decode_unit *decode_unit); // Free decode unit

#endif // DECODE_H
