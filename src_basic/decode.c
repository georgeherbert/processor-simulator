#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "decode.h"
#include "rv32i.h"
#include "control.h"

struct decode_unit *decode_init(
    uint32_t *regs,
    uint32_t *reg_inst,
    uint8_t *reg_rd_addr,
    uint32_t *reg_rs1_val,
    uint32_t *reg_rs2_val,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb)
{
    struct decode_unit *decode_unit = malloc(sizeof(struct decode_unit));
    if (decode_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for decode unit\n");
        exit(EXIT_FAILURE);
    }

    decode_unit->regs = regs;
    decode_unit->reg_inst = reg_inst;
    decode_unit->reg_rd_addr = reg_rd_addr;
    decode_unit->reg_rs1_val = reg_rs1_val;
    decode_unit->reg_rs2_val = reg_rs2_val;
    decode_unit->reg_imm = reg_imm;
    decode_unit->ctrl_src_a = ctrl_src_a;
    decode_unit->ctrl_src_b = ctrl_src_b;
    decode_unit->ctrl_alu_op = ctrl_alu_op;
    decode_unit->ctrl_cmp = ctrl_cmp;
    decode_unit->ctrl_mem = ctrl_mem;
    decode_unit->ctrl_wb = ctrl_wb;

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

uint32_t get_rd_addr(uint32_t inst)
{
    return (inst >> 7) & 0x1F;
}

uint32_t get_rs1_addr(uint32_t inst)
{
    return (inst >> 15) & 0x1F;
}

uint32_t get_rs2_addr(uint32_t inst)
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

void set_ctrl(
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb,
    enum ctrl_src_a src_a,
    enum ctrl_src_b src_b,
    enum ctrl_alu_op alu_op,
    enum ctrl_cmp cmp,
    enum ctrl_mem mem,
    enum ctrl_wb wb)
{
    *ctrl_src_a = src_a;
    *ctrl_src_b = src_b;
    *ctrl_alu_op = alu_op;
    *ctrl_cmp = cmp;
    *ctrl_mem = mem;
    *ctrl_wb = wb;
}

void handle_op_imm(
    uint32_t inst,
    uint32_t *regs,
    uint8_t *reg_rd_addr,
    uint32_t *reg_rs1_val,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb)
{
    *reg_rd_addr = get_rd_addr(inst);
    uint8_t rs1_addr = get_rs1_addr(inst);
    *reg_rs1_val = regs[rs1_addr];

    uint32_t imm = get_imm_i(inst);
    uint32_t shamt = imm & 0x1F;

    enum ctrl_alu_op alu_op;

    switch (get_funct3(inst))
    {
    case FUNCT3_ADDI:
        *reg_imm = imm;
        alu_op = CTRL_ALU_OP_ADD;
        printf("addi x%d, x%d, %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_SLTI:
        *reg_imm = imm;
        alu_op = CTRL_ALU_OP_SLT;
        printf("slti x%d, x%d, %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_SLTIU:
        *reg_imm = imm;
        alu_op = CTRL_ALU_OP_SLTU;
        printf("sltiu x%d, x%d, %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_XORI:
        *reg_imm = imm;
        alu_op = CTRL_ALU_OP_XOR;
        printf("xori x%d, x%d, %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_ORI:
        *reg_imm = imm;
        alu_op = CTRL_ALU_OP_OR;
        printf("ori x%d, x%d, %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_ANDI:
        *reg_imm = imm;
        alu_op = CTRL_ALU_OP_AND;
        printf("andi x%d, x%d, %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_SLLI:
        *reg_imm = shamt;
        alu_op = CTRL_ALU_OP_SLL;
        printf("slli x%d, x%d, %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_SRLI_SRAI:
        switch (get_funct7(inst))
        {
        case FUNCT7_SRLI:
            *reg_imm = shamt;
            alu_op = CTRL_ALU_OP_SRL;
            printf("srli x%d, x%d, %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
            break;
        case FUNCT7_SRAI:
            *reg_imm = shamt;
            alu_op = CTRL_ALU_OP_SRA;
            printf("srai x%d, x%d, %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
            break;
        default:
            fprintf(stderr, "Error: Unknown funct7");
            exit(EXIT_FAILURE);
        }
        break;
    default:
        fprintf(stderr, "Error: Unknown funct3");
        exit(EXIT_FAILURE);
    }

    set_ctrl(
        ctrl_src_a,
        ctrl_src_b,
        ctrl_alu_op,
        ctrl_cmp,
        ctrl_mem,
        ctrl_wb,
        CTRL_SRC_A_RS1,
        CTRL_SRC_B_IMM,
        alu_op,
        CTRL_CMP_NONE,
        CTRL_MEM_NONE,
        CTRL_WB_ALU_OUT);
}

void handle_lui(
    uint32_t inst,
    uint8_t *reg_rd_addr,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb)
{
    *reg_rd_addr = get_rd_addr(inst);
    *reg_imm = get_imm_u(inst);

    set_ctrl(
        ctrl_src_a,
        ctrl_src_b,
        ctrl_alu_op,
        ctrl_cmp,
        ctrl_mem,
        ctrl_wb,
        CTRL_SRC_A_ZERO,
        CTRL_SRC_B_IMM,
        CTRL_ALU_OP_ADD,
        CTRL_CMP_NONE,
        CTRL_MEM_NONE,
        CTRL_WB_ALU_OUT);

    printf("lui x%d, %d", *reg_rd_addr, *reg_imm);
}

void handle_auipc(
    uint32_t inst,
    uint8_t *reg_rd_addr,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb)
{
    *reg_rd_addr = get_rd_addr(inst);
    *reg_imm = get_imm_u(inst);

    set_ctrl(
        ctrl_src_a,
        ctrl_src_b,
        ctrl_alu_op,
        ctrl_cmp,
        ctrl_mem,
        ctrl_wb,
        CTRL_SRC_A_PC,
        CTRL_SRC_B_IMM,
        CTRL_ALU_OP_ADD,
        CTRL_CMP_NONE,
        CTRL_MEM_NONE,
        CTRL_WB_ALU_OUT);

    printf("auipc x%d, %d", *reg_rd_addr, *reg_imm);
}

void handle_op(
    uint32_t inst,
    uint32_t *regs,
    uint8_t *reg_rd_addr,
    uint32_t *reg_rs1_val,
    uint32_t *reg_rs2_val,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb)
{
    *reg_rd_addr = get_rd_addr(inst);
    uint8_t rs1_addr = get_rs1_addr(inst);
    uint8_t rs2_addr = get_rs2_addr(inst);
    *reg_rs1_val = regs[rs1_addr];
    *reg_rs2_val = regs[rs2_addr];

    enum ctrl_alu_op alu_op;

    switch (get_funct3(inst))
    {
    case FUNCT3_ADD_SUB:
        switch (get_funct7(inst))
        {
        case FUNCT7_ADD:
            alu_op = CTRL_ALU_OP_ADD;
            printf("add x%d, x%d, x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
            break;
        case FUNCT7_SUB:
            alu_op = CTRL_ALU_OP_SUB;
            printf("sub x%d, x%d, x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
            break;
        default:
            fprintf(stderr, "Error: Unknown funct7");
            exit(EXIT_FAILURE);
        }
        break;
    case FUNCT3_SLL:
        alu_op = CTRL_ALU_OP_SLL;
        printf("sll x%d, x%d, x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    case FUNCT3_SLT:
        alu_op = CTRL_ALU_OP_SLT;
        printf("slt x%d, x%d, x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    case FUNCT3_SLTU:
        alu_op = CTRL_ALU_OP_SLTU;
        printf("sltu x%d, x%d, x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    case FUNCT3_XOR:
        alu_op = CTRL_ALU_OP_XOR;
        printf("xor x%d, x%d, x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    case FUNCT3_SRL_SRA:
        switch (get_funct7(inst))
        {
        case FUNCT7_SRL:
            alu_op = CTRL_ALU_OP_SRL;
            printf("srl x%d, x%d, x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
            break;
        case FUNCT7_SRA:
            alu_op = CTRL_ALU_OP_SRA;
            printf("sra x%d, x%d, x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
            break;
        default:
            fprintf(stderr, "Error: Unknown funct7");
            exit(EXIT_FAILURE);
        }
        break;
    case FUNCT3_OR:
        alu_op = CTRL_ALU_OP_OR;
        printf("or x%d, x%d, x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    case FUNCT3_AND:
        alu_op = CTRL_ALU_OP_AND;
        printf("and x%d, x%d, x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    default:
        fprintf(stderr, "Error: Unknown funct3");
        exit(EXIT_FAILURE);
    }

    set_ctrl(
        ctrl_src_a,
        ctrl_src_b,
        ctrl_alu_op,
        ctrl_cmp,
        ctrl_mem,
        ctrl_wb,
        CTRL_SRC_A_RS1,
        CTRL_SRC_B_RS2,
        alu_op,
        CTRL_CMP_NONE,
        CTRL_MEM_NONE,
        CTRL_WB_ALU_OUT);
}

void handle_jal(
    uint32_t inst,
    uint8_t *reg_rd_addr,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb)
{
    *reg_rd_addr = get_rd_addr(inst);
    *reg_imm = get_imm_j(inst);

    set_ctrl(
        ctrl_src_a,
        ctrl_src_b,
        ctrl_alu_op,
        ctrl_cmp,
        ctrl_mem,
        ctrl_wb,
        CTRL_SRC_A_PC,
        CTRL_SRC_B_IMM,
        CTRL_ALU_OP_ADD,
        CTRL_CMP_UNCONDITIONAL,
        CTRL_MEM_NONE,
        CTRL_WB_NPC);

    printf("jal x%d, %d\n", *reg_rd_addr, *reg_imm);
}

void handle_jalr(
    uint32_t inst,
    uint32_t *regs,
    uint8_t *reg_rd_addr,
    uint32_t *reg_rs1_val,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb)
{
    *reg_rd_addr = get_rd_addr(inst);
    uint8_t rs1_addr = get_rs1_addr(inst);
    *reg_rs1_val = regs[rs1_addr];
    *reg_imm = get_imm_i(inst);

    set_ctrl(
        ctrl_src_a,
        ctrl_src_b,
        ctrl_alu_op,
        ctrl_cmp,
        ctrl_mem,
        ctrl_wb,
        CTRL_SRC_A_RS1,
        CTRL_SRC_B_IMM,
        CTRL_ALU_OP_ADD_CLEAR_BIT_0,
        CTRL_CMP_UNCONDITIONAL,
        CTRL_MEM_NONE,
        CTRL_WB_NPC);

    printf("jalr x%d, x%d, %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
}

void handle_branch(
    uint32_t inst,
    uint32_t *regs,
    uint32_t *reg_rs1_val,
    uint32_t *reg_rs2_val,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb)
{
    uint8_t rs1_addr = get_rs1_addr(inst);
    uint8_t rs2_addr = get_rs2_addr(inst);
    *reg_rs1_val = regs[rs1_addr];
    *reg_rs2_val = regs[rs2_addr];
    *reg_imm = get_imm_b(inst);

    enum ctrl_cmp cmp;

    switch (get_funct3(inst))
    {
    case FUNCT3_BEQ:
        cmp = CTRL_CMP_EQ;
        printf("beq x%d, x%d, %d\n", rs1_addr, rs2_addr, *reg_imm);
        break;
    case FUNCT3_BNE:
        cmp = CTRL_CMP_NE;
        printf("bne x%d, x%d, %d\n", rs1_addr, rs2_addr, *reg_imm);
        break;
    case FUNCT3_BLT:
        cmp = CTRL_CMP_LT;
        printf("blt x%d, x%d, %d\n", rs1_addr, rs2_addr, *reg_imm);
        break;
    case FUNCT3_BLTU:
        cmp = CTRL_CMP_LTU;
        printf("bltu x%d, x%d, %d\n", rs1_addr, rs2_addr, *reg_imm);
        break;
    case FUNCT3_BGE:
        cmp = CTRL_CMP_GE;
        printf("bge x%d, x%d, %d\n", rs1_addr, rs2_addr, *reg_imm);
        break;
    case FUNCT3_BGEU:
        cmp = CTRL_CMP_GEU;
        printf("bgeu x%d, x%d, %d\n", rs1_addr, rs2_addr, *reg_imm);
        break;
    default:
        printf("Unknown funct3");
        exit(EXIT_FAILURE);
    }

    set_ctrl(
        ctrl_src_a,
        ctrl_src_b,
        ctrl_alu_op,
        ctrl_cmp,
        ctrl_mem,
        ctrl_wb,
        CTRL_SRC_A_PC,
        CTRL_SRC_B_IMM,
        CTRL_ALU_OP_ADD,
        cmp,
        CTRL_MEM_NONE,
        CTRL_WB_NONE);
}

void handle_load(
    uint32_t inst,
    uint32_t *regs,
    uint8_t *reg_rd_addr,
    uint32_t *reg_rs1_val,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb)
{
    *reg_rd_addr = get_rd_addr(inst);
    uint8_t rs1_addr = get_rs1_addr(inst);
    *reg_rs1_val = regs[rs1_addr];
    *reg_imm = get_imm_i(inst);

    enum ctrl_mem mem;

    switch (get_funct3(inst))
    {
    case FUNCT3_LB:
        mem = CTRL_MEM_LOAD_BYTE;
        printf("lb x%d, %d(x%d)\n", *reg_rd_addr, *reg_imm, rs1_addr);
        break;
    case FUNCT3_LBU:
        mem = CTRL_MEM_LOAD_BYTE_UNSIGNED;
        printf("lbu x%d, %d(x%d)\n", *reg_rd_addr, *reg_imm, rs1_addr);
        break;
    case FUNCT3_LH:
        mem = CTRL_MEM_LOAD_HALF;
        printf("lh x%d, %d(x%d)\n", *reg_rd_addr, *reg_imm, rs1_addr);
        break;
    case FUNCT3_LHU:
        mem = CTRL_MEM_LOAD_HALF_UNSIGNED;
        printf("lhu x%d, %d(x%d)\n", *reg_rd_addr, *reg_imm, rs1_addr);
        break;
    case FUNCT3_LW:
        mem = CTRL_MEM_LOAD_WORD;
        printf("lw x%d, %d(x%d)\n", *reg_rd_addr, *reg_imm, rs1_addr);
        break;
    default:
        printf("Unknown funct3");
        exit(EXIT_FAILURE);
    }

    set_ctrl(
        ctrl_src_a,
        ctrl_src_b,
        ctrl_alu_op,
        ctrl_cmp,
        ctrl_mem,
        ctrl_wb,
        CTRL_SRC_A_RS1,
        CTRL_SRC_B_IMM,
        CTRL_ALU_OP_ADD,
        CTRL_CMP_NONE,
        mem,
        CTRL_WB_MDR);
}

void handle_store(
    uint32_t inst,
    uint32_t *regs,
    uint32_t *reg_rs1_val,
    uint32_t *reg_rs2_val,
    uint32_t *reg_imm,
    enum ctrl_src_a *ctrl_src_a,
    enum ctrl_src_b *ctrl_src_b,
    enum ctrl_alu_op *ctrl_alu_op,
    enum ctrl_cmp *ctrl_cmp,
    enum ctrl_mem *ctrl_mem,
    enum ctrl_wb *ctrl_wb)
{
    uint8_t rs1_addr = get_rs1_addr(inst);
    uint8_t rs2_addr = get_rs2_addr(inst);
    *reg_rs1_val = regs[rs1_addr];
    *reg_rs2_val = regs[rs2_addr];
    *reg_imm = get_imm_s(inst);

    enum ctrl_mem mem;

    switch (get_funct3(inst))
    {
    case FUNCT3_SB:
        mem = CTRL_MEM_STORE_BYTE;
        printf("sb x%d, %d(x%d)\n", rs2_addr, *reg_imm, rs1_addr);
        break;
    case FUNCT3_SH:
        mem = CTRL_MEM_STORE_HALF;
        printf("sh x%d, %d(x%d)\n", rs2_addr, *reg_imm, rs1_addr);
        break;
    case FUNCT3_SW:
        mem = CTRL_MEM_STORE_WORD;
        printf("sw x%d, %d(x%d)\n", rs2_addr, *reg_imm, rs1_addr);
        break;
    default:
        printf("Unknown funct3");
        exit(EXIT_FAILURE);
    }

    set_ctrl(
        ctrl_src_a,
        ctrl_src_b,
        ctrl_alu_op,
        ctrl_cmp,
        ctrl_mem,
        ctrl_wb,
        CTRL_SRC_A_RS1,
        CTRL_SRC_B_IMM,
        CTRL_ALU_OP_ADD,
        CTRL_CMP_NONE,
        mem,
        CTRL_WB_NONE);
}

void decode_step(struct decode_unit *decode_unit)
{
    uint32_t inst = *decode_unit->reg_inst;
    uint32_t opcode = get_opcode(inst);

    switch (opcode)
    {
    case OPCODE_OP_IMM:
        handle_op_imm(
            inst,
            decode_unit->regs,
            decode_unit->reg_rd_addr,
            decode_unit->reg_rs1_val,
            decode_unit->reg_imm,
            decode_unit->ctrl_src_a,
            decode_unit->ctrl_src_b,
            decode_unit->ctrl_alu_op,
            decode_unit->ctrl_cmp,
            decode_unit->ctrl_mem,
            decode_unit->ctrl_wb);
        break;
    case OPCODE_LUI:
        handle_lui(
            inst,
            decode_unit->reg_rd_addr,
            decode_unit->reg_imm,
            decode_unit->ctrl_src_a,
            decode_unit->ctrl_src_b,
            decode_unit->ctrl_alu_op,
            decode_unit->ctrl_cmp,
            decode_unit->ctrl_mem,
            decode_unit->ctrl_wb);
        break;
    case OPCODE_AUIPC:
        handle_auipc(
            inst,
            decode_unit->reg_rd_addr,
            decode_unit->reg_imm,
            decode_unit->ctrl_src_a,
            decode_unit->ctrl_src_b,
            decode_unit->ctrl_alu_op,
            decode_unit->ctrl_cmp,
            decode_unit->ctrl_mem,
            decode_unit->ctrl_wb);
        break;
    case OPCODE_OP:
        handle_op(
            inst,
            decode_unit->regs,
            decode_unit->reg_rd_addr,
            decode_unit->reg_rs1_val,
            decode_unit->reg_rs2_val,
            decode_unit->ctrl_src_a,
            decode_unit->ctrl_src_b,
            decode_unit->ctrl_alu_op,
            decode_unit->ctrl_cmp,
            decode_unit->ctrl_mem,
            decode_unit->ctrl_wb);
        break;
    case OPCODE_JAL:
        handle_jal(
            inst,
            decode_unit->reg_rd_addr,
            decode_unit->reg_imm,
            decode_unit->ctrl_src_a,
            decode_unit->ctrl_src_b,
            decode_unit->ctrl_alu_op,
            decode_unit->ctrl_cmp,
            decode_unit->ctrl_mem,
            decode_unit->ctrl_wb);
        break;
    case OPCODE_JALR:
        handle_jalr(
            inst,
            decode_unit->regs,
            decode_unit->reg_rd_addr,
            decode_unit->reg_rs1_val,
            decode_unit->reg_imm,
            decode_unit->ctrl_src_a,
            decode_unit->ctrl_src_b,
            decode_unit->ctrl_alu_op,
            decode_unit->ctrl_cmp,
            decode_unit->ctrl_mem,
            decode_unit->ctrl_wb);
        break;
    case OPCODE_BRANCH:
        handle_branch(
            inst,
            decode_unit->regs,
            decode_unit->reg_rs1_val,
            decode_unit->reg_rs2_val,
            decode_unit->reg_imm,
            decode_unit->ctrl_src_a,
            decode_unit->ctrl_src_b,
            decode_unit->ctrl_alu_op,
            decode_unit->ctrl_cmp,
            decode_unit->ctrl_mem,
            decode_unit->ctrl_wb);
        break;
    case OPCODE_LOAD:
        handle_load(
            inst,
            decode_unit->regs,
            decode_unit->reg_rd_addr,
            decode_unit->reg_rs1_val,
            decode_unit->reg_imm,
            decode_unit->ctrl_src_a,
            decode_unit->ctrl_src_b,
            decode_unit->ctrl_alu_op,
            decode_unit->ctrl_cmp,
            decode_unit->ctrl_mem,
            decode_unit->ctrl_wb);
        break;
    case OPCODE_STORE:
        handle_store(
            inst,
            decode_unit->regs,
            decode_unit->reg_rs1_val,
            decode_unit->reg_rs2_val,
            decode_unit->reg_imm,
            decode_unit->ctrl_src_a,
            decode_unit->ctrl_src_b,
            decode_unit->ctrl_alu_op,
            decode_unit->ctrl_cmp,
            decode_unit->ctrl_mem,
            decode_unit->ctrl_wb);
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
