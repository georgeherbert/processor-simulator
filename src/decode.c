#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "decode.h"
#include "memory.h"
#include "rv32i.h"

struct decode_unit *decode_init(struct memory *memory, uint32_t *reg_inst, uint32_t *reg_rs1, uint32_t *reg_rs2, uint32_t *reg_imm)
{
    struct decode_unit *decode_unit = malloc(sizeof(struct decode_unit));
    if (decode_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for decode unit\n");
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

Instruction Formats:

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

uint32_t get_funct3(uint32_t inst)
{
    return (inst >> 12) & 0x7;
}

uint32_t get_funct7(uint32_t inst)
{
    return (inst >> 25) & 0x7F;
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
    uint32_t imm = (inst >> 20) & 0xFFF;
    if (imm & 0x800)
    {
        imm |= 0xFFFFF000;
    }
    return imm;
}

uint32_t get_imm_s(uint32_t inst)
{
    uint32_t imm = (inst >> 7) & 0x1F;
    imm |= (inst >> 20) & 0xFE0;
    if (imm & 0x800)
    {
        imm |= 0xFFFFF000;
    }
    return imm;
}

uint32_t get_imm_b(uint32_t inst)
{
    uint32_t imm = (inst >> 7) & 0x1E;
    imm |= (inst >> 20) & 0x7E0;
    imm |= (inst << 4) & 0x800;
    imm |= (inst >> 19) & 0x1000;
    if (imm & 0x1000)
    {
        imm |= 0xFFFFE000;
    }
    return imm;
}

uint32_t get_imm_u(uint32_t inst)
{
    return inst & 0xFFFFF000;
}

uint32_t get_imm_j(uint32_t inst)
{
    uint32_t imm = (inst >> 20) & 0x7FE;
    imm |= (inst >> 9) & 0x800;
    imm |= inst & 0xFF000;
    imm |= (inst >> 11) & 0x100000;
    if (imm & 0x100000)
    {
        imm |= 0xFFF00000;
    }
    return imm;
}

void decode_step(struct decode_unit *decode_unit)
{
    uint32_t inst = *decode_unit->reg_inst;
    uint32_t opcode = get_opcode(inst);

    uint32_t funct3;
    uint32_t funct7;

    switch (opcode)
    {
    case OPCODE_OP_IMM:
        funct3 = get_funct3(inst);
        switch (funct3)
        {
        case FUNCT3_ADDI:
            printf("ADDI\n");
            break;
        case FUNCT3_SLTI:
            printf("SLTI\n");
            break;
        case FUNCT3_SLTIU:
            printf("SLTIU\n");
            break;
        case FUNCT3_XORI:
            printf("XORI\n");
            break;
        case FUNCT3_ORI:
            printf("ORI\n");
            break;
        case FUNCT3_ANDI:
            printf("ANDI\n");
            break;
        case FUNCT3_SLLI:
            printf("SLLI\n");
            break;
        case FUNCT3_SRLI_SRAI:
            printf("SRLI_SRAI\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown funct3: %08x", funct3);
            exit(EXIT_FAILURE);
        }
        break;
    case OPCODE_LUI:
        printf("LUI\n");
        break;
    case OPCODE_AUIPC:
        printf("AUIPC\n");
        break;
    case OPCODE_OP:
        funct3 = get_funct3(inst);
        funct7 = get_funct7(inst);
        switch (funct3)
        {
        case FUNCT3_ADD_SUB:
            switch (funct7)
            {
            case FUNCT7_ADD_SRL:
                printf("ADD\n");
                break;
            case FUNCT7_SUB_SRA:
                printf("SUB\n");
                break;
            default:
                fprintf(stderr, "Error: Unknown funct7: %08x", funct7);
                exit(EXIT_FAILURE);
            }
            break;
        case FUNCT3_SLL:
            printf("SLL\n");
            break;
        case FUNCT3_SLT:
            printf("SLT\n");
            break;
        case FUNCT3_SLTU:
            printf("SLTU\n");
            break;
        case FUNCT3_XOR:
            printf("XOR\n");
            break;
        case FUNCT3_SRL_SRA:
            switch (funct7)
            {
            case FUNCT7_ADD_SRL:
                printf("SRL\n");
                break;
            case FUNCT7_SUB_SRA:
                printf("SRA\n");
                break;
            default:
                fprintf(stderr, "Error: Unknown funct7: %08x", funct7);
                exit(EXIT_FAILURE);
            }
            break;
        case FUNCT3_OR:
            printf("OR\n");
            break;
        case FUNCT3_AND:
            printf("AND\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown funct3: %08x", funct3);
            exit(EXIT_FAILURE);
        }
        break;
    case OPCODE_JAL:
        printf("JAL\n");
        break;
    case OPCODE_JALR:
        printf("JALR\n");
        break;
    case OPCODE_BRANCH:
        funct3 = get_funct3(inst);
        switch (funct3)
        {
        case FUNCT3_BEQ:
            printf("BEQ\n");
            break;
        case FUNCT3_BNE:
            printf("BNE\n");
            break;
        case FUNCT3_BLT:
            printf("BLT\n");
            break;
        case FUNCT3_BGE:
            printf("BGE\n");
            break;
        case FUNCT3_BLTU:
            printf("BLTU\n");
            break;
        case FUNCT3_BGEU:
            printf("BGEU\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown funct3: %08x", funct3);
            exit(EXIT_FAILURE);
        }
        break;
    case OPCODE_LOAD:
        funct3 = get_funct3(inst);
        switch (funct3)
        {
        case FUNCT3_LB:
            printf("LB\n");
            break;
        case FUNCT3_LH:
            printf("LH\n");
            break;
        case FUNCT3_LW:
            printf("LW\n");
            break;
        case FUNCT3_LBU:
            printf("LBU\n");
            break;
        case FUNCT3_LHU:
            printf("LHU\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown funct3: %08x", funct3);
            exit(EXIT_FAILURE);
        }
        break;
    case OPCODE_STORE:
        funct3 = get_funct3(inst);
        switch (funct3)
        {
        case FUNCT3_SB:
            printf("SB\n");
            break;
        case FUNCT3_SH:
            printf("SH\n");
            break;
        case FUNCT3_SW:
            printf("SW\n");
            break;
        default:
            fprintf(stderr, "Error: Unknown funct3: %08x", funct3);
            exit(EXIT_FAILURE);
        }
        break;
    default:
        fprintf(stderr, "Error: Unknown opcode: %08x", opcode);
        exit(EXIT_FAILURE);
    }
}

void decode_destroy(struct decode_unit *decode_unit)
{
    free(decode_unit);
}
