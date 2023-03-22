#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "execute.h"
#include "control.h"

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
    enum ctrl_pc_src *ctrl_pc_src)
{
    struct execute_unit *execute_unit = malloc(sizeof(struct execute_unit));
    if (execute_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for execute unit\n");
        exit(EXIT_FAILURE);
    }

    execute_unit->reg_pc = reg_pc;
    execute_unit->reg_rs1_val = reg_rs1_val;
    execute_unit->reg_rs2_val = reg_rs2_val;
    execute_unit->reg_imm = reg_imm;
    execute_unit->ctrl_src_a = ctrl_src_a;
    execute_unit->ctrl_src_b = ctrl_src_b;
    execute_unit->ctrl_alu_op = ctrl_alu_op;
    execute_unit->ctrl_cmp = ctrl_cmp;
    execute_unit->reg_alu_out = reg_alu_out;
    execute_unit->ctrl_pc_src = ctrl_pc_src;

    return execute_unit;
}

uint32_t get_src_a_val(enum ctrl_src_a *ctrl_src_a, uint32_t *reg_rs1_val, uint32_t *reg_pc)
{
    uint32_t src_a;
    switch (*ctrl_src_a)
    {
    case CTRL_SRC_A_RS1:
        src_a = *reg_rs1_val;
        break;
    case CTRL_SRC_A_PC:
        src_a = *reg_pc;
        break;
    case CTRL_SRC_A_ZERO:
        src_a = 0;
        break;
    default:
        fprintf(stderr, "Error: Invalid control signal for source A to ALU\n");
        exit(EXIT_FAILURE);
    }

    return src_a;
}

uint32_t get_src_b_val(enum ctrl_src_b *ctrl_src_b, uint32_t *reg_rs2_val, uint32_t *reg_imm)
{
    uint32_t src_b;
    switch (*ctrl_src_b)
    {
    case CTRL_SRC_B_RS2:
        src_b = *reg_rs2_val;
        break;
    case CTRL_SRC_B_IMM:
        src_b = *reg_imm;
        break;
    default:
        fprintf(stderr, "Error: Invalid control signal for source B to ALU\n");
        exit(EXIT_FAILURE);
    }

    return src_b;
}

// CTRL_ALU_OP_ADD,
// CTRL_ALU_OP_ADD_CLEAR_BIT_0,
// CTRL_ALU_OP_SUB,
// CTRL_ALU_OP_SLL,
// CTRL_ALU_OP_SLT,
// CTRL_ALU_OP_SLTU,
// CTRL_ALU_OP_XOR,
// CTRL_ALU_OP_SRL,
// CTRL_ALU_OP_SRA,
// CTRL_ALU_OP_OR,
// CTRL_ALU_OP_AND,

void set_alu_out(uint32_t src_a, uint32_t src_b, enum ctrl_alu_op *ctrl_alu_op, uint32_t *reg_alu_out)
{
    switch (*ctrl_alu_op)
    {
    case CTRL_ALU_OP_ADD:
        *reg_alu_out = src_a + src_b;
        // printf("%d + %d = %d\n", src_a, src_b, *reg_alu_out);
        break;
    case CTRL_ALU_OP_ADD_CLEAR_BIT_0:
        *reg_alu_out = (src_a + src_b) & ~1;
        // printf("%d + %d (and clear bit zero) = %d\n", src_a, src_b, *reg_alu_out);
        break;
    case CTRL_ALU_OP_SUB:
        *reg_alu_out = src_a - src_b;
        // printf("%d - %d = %d\n", src_a, src_b, *reg_alu_out);
        break;
    case CTRL_ALU_OP_SLL:
        *reg_alu_out = src_a << src_b;
        // printf("%d << %d = %d\n", src_a, src_b, *reg_alu_out);
        break;
    case CTRL_ALU_OP_SLT:
        *reg_alu_out = (int32_t)src_a < (int32_t)src_b;
        // printf("%d < %d = %d\n", src_a, src_b, *reg_alu_out);
        break;
    case CTRL_ALU_OP_SLTU:
        *reg_alu_out = src_a < src_b;
        // printf("%u < %u = %d\n", src_a, src_b, *reg_alu_out);
        break;
    case CTRL_ALU_OP_XOR:
        *reg_alu_out = src_a ^ src_b;
        // printf("%d ^ %d = %d\n", src_a, src_b, *reg_alu_out);
        break;
    case CTRL_ALU_OP_SRL:
        *reg_alu_out = src_a >> src_b;
        // printf("%d >> %d = %d\n", src_a, src_b, *reg_alu_out);
        break;
    case CTRL_ALU_OP_SRA:
        *reg_alu_out = (int32_t)src_a >> src_b;
        // printf("%d >>a %d = %d\n", src_a, src_b, *reg_alu_out);
        break;
    case CTRL_ALU_OP_OR:
        *reg_alu_out = src_a | src_b;
        // printf("%d | %d = %d\n", src_a, src_b, *reg_alu_out);
        break;
    case CTRL_ALU_OP_AND:
        *reg_alu_out = src_a & src_b;
        // printf("%d & %d = %d\n", src_a, src_b, *reg_alu_out);
        break;
    default:
        fprintf(stderr, "Error: Invalid control signal for ALU operation\n");
        exit(EXIT_FAILURE);
    }
}

void set_cmp_out(uint32_t *reg_src_a, uint32_t *reg_src_b, enum ctrl_cmp *ctrl_cmp, uint32_t *ctrl_pc_src)
{
    switch (*ctrl_cmp)
    {
    case CTRL_CMP_NONE:
        *ctrl_pc_src = CTRL_PC_SRC_NPC;
        break;
    case CTRL_CMP_UNCONDITIONAL:
        *ctrl_pc_src = CTRL_PC_SRC_ALU_OUT;
        break;
    case CTRL_CMP_EQ:
        *ctrl_pc_src = *reg_src_a == *reg_src_b ? CTRL_PC_SRC_ALU_OUT : CTRL_PC_SRC_NPC;
        break;
    case CTRL_CMP_NE:
        *ctrl_pc_src = *reg_src_a != *reg_src_b ? CTRL_PC_SRC_ALU_OUT : CTRL_PC_SRC_NPC;
        break;
    case CTRL_CMP_LT:
        *ctrl_pc_src = (int32_t)*reg_src_a < (int32_t)*reg_src_b ? CTRL_PC_SRC_ALU_OUT : CTRL_PC_SRC_NPC;
        break;
    case CTRL_CMP_LTU:
        printf("%d %d\n", *reg_src_a, *reg_src_b);
        *ctrl_pc_src = *reg_src_a < *reg_src_b ? CTRL_PC_SRC_ALU_OUT : CTRL_PC_SRC_NPC;
        break;
    case CTRL_CMP_GE:
        *ctrl_pc_src = (int32_t)*reg_src_a >= (int32_t)*reg_src_b ? CTRL_PC_SRC_ALU_OUT : CTRL_PC_SRC_NPC;
        break;
    case CTRL_CMP_GEU:
        *ctrl_pc_src = *reg_src_a >= *reg_src_b ? CTRL_PC_SRC_ALU_OUT : CTRL_PC_SRC_NPC;
        break;
    default:
        fprintf(stderr, "Error: Invalid control signal for comparison operation\n");
        exit(EXIT_FAILURE);
    }
}

void execute_step(struct execute_unit *execute_unit)
{
    uint32_t src_a = get_src_a_val(execute_unit->ctrl_src_a, execute_unit->reg_rs1_val, execute_unit->reg_pc);
    uint32_t src_b = get_src_b_val(execute_unit->ctrl_src_b, execute_unit->reg_rs2_val, execute_unit->reg_imm);
    set_alu_out(src_a, src_b, execute_unit->ctrl_alu_op, execute_unit->reg_alu_out);
    set_cmp_out(execute_unit->reg_rs1_val, execute_unit->reg_rs2_val, execute_unit->ctrl_cmp, execute_unit->ctrl_pc_src);
}

void execute_destroy(struct execute_unit *execute_unit)
{
    free(execute_unit);
}
