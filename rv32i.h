#ifndef RV32I_H
#define RV32I_H

#include <stdint.h>

typedef enum Opcode
{
    // Integer Register-Immediate
    OP_IMM_OPCODE = 0b0010011,
    LUI_OPCODE = 0b0110111,
    AUIPC_OPCODE = 0b0010111,
    // Integer Register-Register
    OP_OPCODE = 0b0110011,
    // Unconditional Jumps
    JAL_OPCODE = 0b1101111,
    JALR_OPCODE = 0b1100111,
    // Conditional Branches
    BRANCH_OPCODE = 0b1100011,
    // Load and Store
    LOAD_OPCODE = 0b0000011,
    STORE_OPCODE = 0b0100011
} Opcode;

typedef enum Funct3
{
    // Integer Register-Immediate
    ADDI_FUNCT3 = 0b000,
    SLTI_FUNCT3 = 0b010,
    SLTIU_FUNCT3 = 0b011,
    ANDI_FUNCT3 = 0b111,
    ORI_FUNCT3 = 0b110,
    XORI_FUNCT3 = 0b100,
    SLLI_FUNCT3 = 0b001,
    SRLI_FUNCT3 = 0b101,
    SRAI_FUNCT3 = 0b101,
    // Integer Register-Register
    ADD_FUNCT3 = 0b000,
    SLT_FUNCT3 = 0b010,
    SLTU_FUNCT3 = 0b011,
    AND_FUNCT3 = 0b111,
    OR_FUNCT3 = 0b110,
    XOR_FUNCT3 = 0b100,
    SLL_FUNCT3 = 0b001,
    SRL_FUNCT3 = 0b101,
    SUB_FUNCT3 = 0b000,
    SRA_FUNCT3 = 0b101,
    // Unconditional Jumps
    JALR_FUNCT3 = 0b000,
    // Conditional Branches
    BEQ_FUNCT3 = 0b000,
    BNE_FUNCT3 = 0b001,
    BLT_FUNCT3 = 0b100,
    BLTU_FUNCT3 = 0b110,
    BGE_FUNCT3 = 0b101,
    BGEU_FUNCT3 = 0b111,
    // Load and Store
    LW_FUNCT3 = 0b010,
    LH_FUNCT3 = 0b001,
    LHU_FUNCT3 = 0b101,
    LB_FUNCT3 = 0b000,
    LBU_FUNCT3 = 0b100,
    SW_FUNCT3 = 0b010,
    SH_FUNCT3 = 0b001,
    SB_FUNCT3 = 0b000
} Funct3;

typedef enum Funct7
{
    // Integer Register-Register
    ADD_FUNCT7 = 0b0000000,
    SLT_FUNCT7 = 0b0000000,
    SLTU_FUNCT7 = 0b0000000,
    AND_FUNCT7 = 0b0000000,
    OR_FUNCT7 = 0b0000000,
    XOR_FUNCT7 = 0b0000000,
    SLL_FUNCT7 = 0b0000000,
    SRL_FUNCT7 = 0b0000000,
    SUB_FUNCT7 = 0b0100000,
    SRA_FUNCT7 = 0b0100000
} Funct7;

typedef enum ShiftType
{
    // Integer Register-Immediate
    SLLI_SHIFT_TYPE = 0b0000000,
    SRLI_SHIFT_TYPE = 0b0000000,
    SRAI_SHIFT_TYPE = 0b0100000
} ShiftType;

typedef union rv32i_instruction
{
    struct
    {
        Opcode opcode : 7;
        uint32_t rd : 5;
        Funct3 funct3 : 3;
        uint32_t rs1 : 5;
        uint32_t rs2 : 5;
        Funct7 funct7 : 7;
    } r_type;

    struct
    {
        Opcode opcode : 7;
        uint32_t rd : 5;
        Funct3 funct3 : 3;
        uint32_t rs1 : 5;
        uint32_t imm : 12;
    } i_type;

    struct
    {
        Opcode opcode : 7;
        uint32_t rd : 5;
        Funct3 funct3 : 3;
        uint32_t rs1 : 5;
        uint32_t shamt : 5;
        ShiftType shift_type : 7;
    } i_type_shift;

    struct
    {
        Opcode opcode : 7;
        uint32_t imm : 12;
        Funct3 funct3 : 3;
        uint32_t rs1 : 5;
        uint32_t rs2 : 5;
    } s_type;

    struct
    {
        Opcode opcode : 7;
        uint32_t rd : 5;
        uint32_t imm : 20;
    } u_type;

} rv32i_instruction;

#endif // RV32I_H