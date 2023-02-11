#include <stdio.h>
#include <stdint.h>
#include "rv32i.h"

int main(int argc, char *argv[])
{
    FILE *fp;

    uint32_t num_words = 5;
    rv32i_instruction machine_code[num_words];

    machine_code[1].u_type.opcode = 12;
    machine_code[1].u_type.rd = 3;
    machine_code[1].u_type.imm = 4;

    machine_code[0].u_type.imm = 100;
    machine_code[0].u_type.rd = 5;
    machine_code[0].u_type.opcode = 17;

    if (argc != 2)
    {
        printf("Usage: %s <output_file>\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "wb");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    for (uint32_t i = 0; i < num_words; i++)
    {
        fwrite(&machine_code[i], sizeof(rv32i_instruction), 1, fp);
    }

    fclose(fp);
    return 0;
}
