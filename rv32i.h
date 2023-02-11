#ifndef RV32I_H
#define RV32I_H

#include <stdint.h>

typedef union rv32i_instruction
{
    struct
    {
        uint32_t opcode : 7;
        uint32_t rd : 5;
        uint32_t funct3 : 3;
        uint32_t rs1 : 5;
        uint32_t rs2 : 5;
        uint32_t funct7 : 7;
    } r_type;

    struct
    {
        uint32_t opcode : 7;
        uint32_t rd : 5;
        uint32_t funct3 : 3;
        uint32_t rs1 : 5;
        int32_t imm : 12;
    } i_type;

    struct
    {
        uint32_t opcode : 7;
        int32_t imm : 12;
        uint32_t funct3 : 3;
        uint32_t rs1 : 5;
        uint32_t rs2 : 5;
    } s_type;

    struct
    {
        uint32_t opcode : 7;
        uint32_t rd : 5;
        int32_t imm : 20;
    } u_type;

} rv32i_instruction;

#endif // RV32I_H