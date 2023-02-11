#include <stdio.h>
#include <stdint.h>
#include "rv32i.h"

#define NUM_REGISTERS 32

uint32_t registers[NUM_REGISTERS];
uint32_t pc = 0;
rv32i_instruction *instructions;

int main(int argc, char *argv[])
{
    FILE *fp;
    rv32i_instruction s;

    fp = fopen("test.o", "rb");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    while (fread(&s, sizeof(rv32i_instruction), 1, fp) == 1)
    {
        printf("opcode: %d\n", s.u_type.opcode);
        printf("rd: %d\n", s.u_type.rd);
        printf("imm: %d\n", s.u_type.imm);
    }

    fclose(fp);
    return 0;
}
