#ifndef RV32I_H
#define RV32I_H

#include <stdint.h>

typedef enum Opcode
{
    ADDI,
    SLTI,
    SLTIU,
    ANDI,
    ORI,
    XORI,
    SLLI,
    SRLI,
    SRAI,
    LUI,
    AUIPC,
    ADD,
    SLT,
    SLTU,
    AND,
    OR,
    XOR,
    SLL,
    SRL,
    SUB,
    SRA,
    JAL,
    JALR,
    BEQ,
    BNE,
    BLT,
    BLTU,
    BGE,
    BGEU,
    LW,
    LH,
    LHU,
    LB,
    LBU,
    SW,
    SH,
    SB
} Opcode;

typedef union rv32i_t
{
    struct
    {
        Opcode opcode : 7;
        uint32_t rd : 5;
        uint32_t rs1 : 5;
        int32_t imm : 12;
    } i_type;

    struct
    {
        Opcode opcode : 7;
        uint32_t rd : 5;
        uint32_t rs1 : 5;
        uint32_t shamt : 5;
    } i_type_shift;

    struct
    {
        Opcode opcode : 7;
        uint32_t rd : 5;
        int32_t imm : 20;
    } u_type;

    struct
    {
        Opcode opcode : 7;
        uint32_t rd : 5;
        uint32_t rs1 : 5;
        uint32_t rs2 : 5;
    } r_type;

    struct
    {
        Opcode opcode : 7;
        uint32_t rs1 : 5;
        uint32_t rs2 : 5;
        int32_t imm : 12;
    } s_type;

} rv32i_t;

#endif // RV32I_H