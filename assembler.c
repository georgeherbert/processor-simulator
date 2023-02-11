#include <stdio.h>
#include <stdint.h>
#include "rv32i.h"

int main(int argc, char *argv[])
{
    FILE *fp;
    rv32i_instruction s;

    if (argc != 2)
    {
        printf("Usage: %s <output_file>\n", argv[0]);
        return 1;
    }

    s.u_type.opcode = 12;
    s.u_type.rd = 3;
    s.u_type.imm = 4;

    fp = fopen(argv[1], "wb");
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    fwrite(&s, sizeof(rv32i_instruction), 1, fp);

    fclose(fp);
    return 0;
}
