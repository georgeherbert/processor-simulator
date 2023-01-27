#include <stdio.h>
#include <stdint.h>

#define NUM_REGISTERS 32

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

uint32_t registers[NUM_REGISTERS];
uint32_t pc = 0;
rv32i_instruction *instructions;

int main(int argc, char *argv[])
{
    return 0;
}
