#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdint.h>
#include "control.h"

struct execute_unit
{
    uint32_t *reg_pc;              // Pointer to program counter register
    uint32_t *reg_rs1_val;         // Pointer to source register 1
    uint32_t *reg_rs2_val;         // Pointer to source register 2
    uint32_t *reg_imm;             // Pointer to immediate register
    enum ctrl_src_a *ctrl_src_a;   // Pointer to control signal for source A to ALU
    enum ctrl_src_b *ctrl_src_b;   // Pointer to control signal for source B to ALU
    enum ctrl_alu_op *ctrl_alu_op; // Pointer to control signal for ALU operation
    enum ctrl_cmp *ctrl_cmp;       // Pointer to control signal for ALU comparison operation

    uint32_t *reg_alu_out;         // Pointer to ALU operation output register
    enum ctrl_pc_src *ctrl_pc_src; // Pointer to control signal for PC source
};

struct execute_unit *execute_init(
    uint32_t *reg_pc,
    uint32_t *reg_rs1_val,
    uint32_t *reg_rs2_val,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    uint32_t *reg_alu_out,
    enum ctrl_pc_src *ctrl_pc_src);                      // Initialise execute unit
void execute_step(struct execute_unit *execute_unit);    // Step execute unit
void execute_destroy(struct execute_unit *execute_unit); // Free execute unit

#endif // EXECUTE_H
