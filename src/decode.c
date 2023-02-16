#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "decode.h"
#include "memory.h"

struct decode_unit *decode_init(struct memory *memory, uint32_t *reg_inst, uint32_t *reg_rs1, uint32_t *reg_rs2, uint32_t *reg_imm)
{
    struct decode_unit *decode_unit = malloc(sizeof(struct decode_unit));
    if (decode_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for decode unit");
        exit(EXIT_FAILURE);
    }

    decode_unit->memory = memory;
    decode_unit->reg_inst = reg_inst;
    decode_unit->reg_rs1 = reg_rs1;
    decode_unit->reg_rs2 = reg_rs2;
    decode_unit->reg_imm = reg_imm;

    return decode_unit;
}

/*

R-type
 31                25 24          20 19          15 14    12 11           7 6                  0
+--------------------+--------------+--------------+--------+--------------+--------------------+
| funct7             | rs2          | rs1          | funct3 | rd           | opcode             |
+--------------------+--------------+--------------+--------+--------------+--------------------+

I-type
 31                               20 19          15 14    12 11           7 6                  0
+-----------------------------------+--------------+--------+--------------+--------------------+
| imm[11:0]                         | rs1          | funct3 | rd           | opcode             |
+-----------------------------------+--------------+--------+--------------+--------------------+

S-type
 31                25 24          20 19          15 14    12 11           7 6                  0
+--------------------+--------------+--------------+--------+--------------+--------------------+
| imm[11:5]          | rs2          | rs1          | funct3 | imm[4:0]     | opcode             |
+--------------------+--------------+--------------+--------+--------------+--------------------+

B-type
 31                25 24          20 19          15 14    12 11           7 6                  0
+--------------------+--------------+--------------+--------+--------------+--------------------+
| imm[12|10:5]       | rs2          | rs1          | funct3 | imm[4:1|11]  | opcode             |
+--------------------+--------------+--------------+--------+--------------+--------------------+

U-type
 31                                                       12 11           7 6                  0
+-----------------------------------------------------------+--------------+--------------------+
| imm[31:12]                                                | rd           | opcode             |
+-----------------------------------------------------------+--------------+--------------------+

J-type
 31                                                       12 11           7 6                  0
+-----------------------------------------------------------+--------------+--------------------+
| imm[20|10:1|11|19:12]                                     | rd           | opcode             |
+-----------------------------------------------------------+--------------+--------------------+

*/

uint32_t get_opcode(uint32_t inst)
{
    return inst & 0x7F;
}

uint32_t get_rs1(uint32_t inst)
{
    return (inst >> 15) & 0x1F;
}

uint32_t get_rs2(uint32_t inst)
{
    return (inst >> 20) & 0x1F;
}

uint32_t get_imm_i(uint32_t inst)
{
    return (inst >> 20) & 0xFFF;
}

uint32_t get_imm_s(uint32_t inst)
{
    uint32_t imm = (inst >> 7) & 0x1F;
    imm |= (inst >> 20) & 0xFE0;
    return imm;
}

uint32_t get_imm_b(uint32_t inst)
{
    uint32_t imm = (inst >> 8) & 0xF;
    imm |= (inst >> 20) & 0x7E0;
    imm |= (inst >> 7) & 0x800;
    imm |= (inst >> 31) & 0x1000;
    return imm;
}

uint32_t get_imm_u(uint32_t inst)
{
    return (inst >> 12) & 0xFFFFF;
}

uint32_t get_imm_j(uint32_t inst)
{
    uint32_t imm = (inst >> 12) & 0xFF;
    imm |= (inst >> 20) & 0x7FE;
    imm |= (inst >> 21) & 0x800;
    imm |= (inst >> 31) & 0x1000;
    return imm;
}

void decode_step(struct decode_unit *decode_unit)
{
    uint32_t inst = *decode_unit->reg_inst;
    uint32_t opcode = get_opcode(inst);

    printf("inst: %08x, opcode: %08x\n", inst, opcode);
}

void decode_destroy(struct decode_unit *decode_unit)
{
    free(decode_unit);
}
