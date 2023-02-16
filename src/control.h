#ifndef CONTROL_H
#define CONTROL_H

enum src_a
{
    SRC_A_RS1,
    SRC_A_PC,
    SRC_A_ZERO
};

enum src_b
{
    SRC_B_RS2,
    SRC_B_IMM,
};

enum alu_op
{
    ALU_OP_ADD,
    ALU_OP_ADD_MOD_2,
    ALU_OP_SUB,
    ALU_OP_SLL,
    ALU_OP_SLT,
    ALU_OP_SLTU,
    ALU_OP_XOR,
    ALU_OP_SRL,
    ALU_OP_SRA,
    ALU_OP_OR,
    ALU_OP_AND,
};

enum alu_cmp
{
    ALU_CMP_NONE,
    ALU_CMP_UNCONDITIONAL,
    ALU_CMP_EQ,
    ALU_CMP_NE,
    ALU_CMP_LT,
    ALU_CMP_LTU,
    ALU_CMP_GE,
    ALU_CMP_GEU,
};

enum mem_op
{
    MEM_OP_NONE,
    MEM_OP_LOAD,
    MEM_OP_STORE,
};

enum wb_op
{
    WB_OP_NONE,
    WB_OP_ALU,
    WB_OP_ALU_PLUS_4,
    WB_OP_MEM,
};

#endif // CONTROL_H
