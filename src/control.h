#ifndef CONTROL_H
#define CONTROL_H

enum ctrl_pc_src
{
    CTRL_PC_SRC_NPC,
    CTRL_PC_SRC_ALU_OUT
};

enum ctrl_src_a
{
    CTRL_SRC_A_RS1,
    CTRL_SRC_A_PC,
    CTRL_SRC_A_ZERO
};

enum ctrl_src_b
{
    CTRL_SRC_B_RS2,
    CTRL_SRC_B_IMM,
};

enum ctrl_alu_op
{
    CTRL_ALU_OP_ADD,
    CTRL_ALU_OP_ADD_CLEAR_BIT_0,
    CTRL_ALU_OP_SUB,
    CTRL_ALU_OP_SLL,
    CTRL_ALU_OP_SLT,
    CTRL_ALU_OP_SLTU,
    CTRL_ALU_OP_XOR,
    CTRL_ALU_OP_SRL,
    CTRL_ALU_OP_SRA,
    CTRL_ALU_OP_OR,
    CTRL_ALU_OP_AND,
};

enum ctrl_cmp
{
    CTRL_CMP_NONE,
    CTRL_CMP_UNCONDITIONAL,
    CTRL_CMP_EQ,
    CTRL_CMP_NE,
    CTRL_CMP_LT,
    CTRL_CMP_LTU,
    CTRL_CMP_GE,
    CTRL_CMP_GEU,
};

enum ctrl_mem
{
    CTRL_MEM_NONE,
    CTRL_MEM_LOAD_WORD,
    CTRL_MEM_LOAD_HALF,
    CTRL_MEM_LOAD_HALF_UNSIGNED,
    CTRL_MEM_LOAD_BYTE,
    CTRL_MEM_LOAD_BYTE_UNSIGNED,
    CTRL_MEM_STORE_WORD,
    CTRL_MEM_STORE_HALF,
    CTRL_MEM_STORE_BYTE,
};

enum ctrl_wb
{
    CTRL_WB_NONE,
    CTRL_WB_ALU_OUT,
    CTRL_WB_MDR,
    CTRL_WB_NPC
};

#endif // CONTROL_H
