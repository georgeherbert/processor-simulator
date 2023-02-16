#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "decode.h"
#include "rv32i.h"
#include "control.h"

struct decode_unit *decode_init(
    uint32_t *regs,
    uint32_t *reg_inst,
    uint32_t *reg_rs1_val,
    uint32_t *reg_rs2_val,
    uint32_t *reg_imm,
    uint8_t *reg_rd_addr,
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b)
{
    struct decode_unit *decode_unit = malloc(sizeof(struct decode_unit));
    if (decode_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for decode unit\n");
        exit(EXIT_FAILURE);
    }

    decode_unit->regs = regs;
    decode_unit->reg_inst = reg_inst;
    decode_unit->reg_rs1_val = reg_rs1_val;
    decode_unit->reg_rs2_val = reg_rs2_val;
    decode_unit->reg_imm = reg_imm;
    decode_unit->reg_rd_addr = reg_rd_addr;
    decode_unit->reg_ctrl_alu_op = reg_ctrl_alu_op;
    decode_unit->reg_ctrl_alu_cmp = reg_ctrl_alu_cmp;
    decode_unit->reg_ctrl_alu_src_a = reg_ctrl_alu_src_a;
    decode_unit->reg_ctrl_alu_src_b = reg_ctrl_alu_src_b;

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
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b,
    uint8_t ctrl_alu_op,
    uint8_t ctrl_alu_cmp,
    uint8_t ctrl_alu_src_a,
    uint8_t ctrl_alu_src_b)
{
    *reg_ctrl_alu_op = ctrl_alu_op;
    *reg_ctrl_alu_cmp = ctrl_alu_cmp;
    *reg_ctrl_alu_src_a = ctrl_alu_src_a;
    *reg_ctrl_alu_src_b = ctrl_alu_src_b;
}

void set_ctrl_op_imm(
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b,
    uint8_t ctrl_alu_op)
{
    set_ctrl(
        reg_ctrl_alu_op,
        reg_ctrl_alu_cmp,
        reg_ctrl_alu_src_a,
        reg_ctrl_alu_src_b,
        ctrl_alu_op,
        ALU_CMP_NONE,
        SRC_A_RS1,
        SRC_B_IMM);
}

void handle_op_imm(
    uint32_t inst,
    uint32_t *regs,
    uint32_t *reg_rs1_val,
    uint32_t *reg_imm,
    uint8_t *reg_rd_addr,
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b)
{
    uint32_t imm = get_imm_i(inst);
    uint32_t shamt = imm & 0x1F;

    uint8_t rs1_addr = get_rs1_addr(inst);
    *reg_rs1_val = regs[rs1_addr];
    *reg_rd_addr = get_rd_addr(inst);

    switch (get_funct3(inst))
    {
    case FUNCT3_ADDI:
        *reg_imm = imm;
        set_ctrl_op_imm(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_ADD);
        printf("addi x%d x%d %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_SLTI:
        *reg_imm = imm;
        set_ctrl_op_imm(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SLT);
        printf("slti x%d x%d %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_SLTIU:
        *reg_imm = imm;
        set_ctrl_op_imm(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SLTU);
        printf("sltiu x%d x%d %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_XORI:
        *reg_imm = imm;
        set_ctrl_op_imm(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_XOR);
        printf("xori x%d x%d %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_ORI:
        *reg_imm = imm;
        set_ctrl_op_imm(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_OR);
        printf("ori x%d x%d %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_ANDI:
        *reg_imm = imm;
        set_ctrl_op_imm(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_AND);
        printf("andi x%d x%d %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_SLLI:
        *reg_imm = shamt;
        set_ctrl_op_imm(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SLL);
        printf("slli x%d x%d %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
        break;
    case FUNCT3_SRLI_SRAI:
        switch (get_funct7(inst))
        {
        case FUNCT7_SRLI:
            *reg_imm = shamt;
            set_ctrl_op_imm(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SRL);
            printf("srli x%d x%d %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
            break;
        case FUNCT7_SRAI:
            *reg_imm = shamt;
            set_ctrl_op_imm(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SRA);
            printf("srai x%d x%d %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
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
}

void handle_lui(
    uint32_t inst,
    uint32_t *reg_imm,
    uint8_t *reg_rd_addr,
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b)
{
    *reg_imm = get_imm_u(inst);
    *reg_rd_addr = get_rd_addr(inst);

    set_ctrl(
        reg_ctrl_alu_op,
        reg_ctrl_alu_cmp,
        reg_ctrl_alu_src_a,
        reg_ctrl_alu_src_b,
        ALU_OP_ADD,
        ALU_CMP_NONE,
        SRC_A_ZERO,
        SRC_B_IMM);

    printf("lui x%d %d", *reg_rd_addr, *reg_imm);
}

void handle_auipc(
    uint32_t inst,
    uint32_t *reg_imm,
    uint8_t *reg_rd_addr,
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b)
{
    *reg_imm = get_imm_u(inst);
    *reg_rd_addr = get_rd_addr(inst);

    set_ctrl(
        reg_ctrl_alu_op,
        reg_ctrl_alu_cmp,
        reg_ctrl_alu_src_a,
        reg_ctrl_alu_src_b,
        ALU_OP_ADD,
        ALU_CMP_NONE,
        SRC_A_PC,
        SRC_B_IMM);

    printf("auipc x%d %d", *reg_rd_addr, *reg_imm);
}

void set_ctrl_op(
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b,
    uint8_t ctrl_alu_op)
{
    set_ctrl(
        reg_ctrl_alu_op,
        reg_ctrl_alu_cmp,
        reg_ctrl_alu_src_a,
        reg_ctrl_alu_src_b,
        ctrl_alu_op,
        ALU_CMP_NONE,
        SRC_A_RS1,
        SRC_B_RS2);
}

void handle_op(
    uint32_t inst,
    uint32_t *regs,
    uint32_t *reg_rs1_val,
    uint32_t *reg_rs2_val,
    uint8_t *reg_rd_addr,
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b)
{
    uint8_t rs1_addr = get_rs1_addr(inst);
    uint8_t rs2_addr = get_rs2_addr(inst);
    *reg_rs1_val = regs[rs1_addr];
    *reg_rs2_val = regs[rs2_addr];
    *reg_rd_addr = get_rd_addr(inst);

    switch (get_funct3(inst))
    {
    case FUNCT3_ADD_SUB:
        switch (get_funct7(inst))
        {
        case FUNCT7_ADD:
            set_ctrl_op(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_ADD);
            printf("add x%d x%d x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
            break;
        case FUNCT7_SUB:
            set_ctrl_op(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SUB);
            printf("sub x%d x%d x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
            break;
        default:
            fprintf(stderr, "Error: Unknown funct7");
            exit(EXIT_FAILURE);
        }
        break;
    case FUNCT3_SLL:
        set_ctrl_op(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SLL);
        printf("sll x%d x%d x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    case FUNCT3_SLT:
        set_ctrl_op(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SLT);
        printf("slt x%d x%d x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    case FUNCT3_SLTU:
        set_ctrl_op(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SLTU);
        printf("sltu x%d x%d x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    case FUNCT3_XOR:
        set_ctrl_op(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_XOR);
        printf("xor x%d x%d x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    case FUNCT3_SRL_SRA:
        switch (get_funct7(inst))
        {
        case FUNCT7_SRL:
            set_ctrl_op(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SRL);
            printf("srl x%d x%d x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
            break;
        case FUNCT7_SRA:
            set_ctrl_op(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_SRA);
            printf("sra x%d x%d x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
            break;
        default:
            fprintf(stderr, "Error: Unknown funct7");
            exit(EXIT_FAILURE);
        }
        break;
    case FUNCT3_OR:
        set_ctrl_op(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_OR);
        printf("or x%d x%d x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    case FUNCT3_AND:
        set_ctrl_op(reg_ctrl_alu_op, reg_ctrl_alu_cmp, reg_ctrl_alu_src_a, reg_ctrl_alu_src_b, ALU_OP_AND);
        printf("and x%d x%d x%d\n", *reg_rd_addr, rs1_addr, rs2_addr);
        break;
    default:
        fprintf(stderr, "Error: Unknown funct3");
        exit(EXIT_FAILURE);
    }
}

void handle_jal(
    uint32_t inst,
    uint32_t *reg_imm,
    uint8_t *reg_rd_addr,
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b)
{
    *reg_imm = get_imm_j(inst);
    *reg_rd_addr = get_rd_addr(inst);

    set_ctrl(
        reg_ctrl_alu_op,
        reg_ctrl_alu_cmp,
        reg_ctrl_alu_src_a,
        reg_ctrl_alu_src_b,
        ALU_OP_ADD,
        ALU_CMP_UNCONDITIONAL,
        SRC_A_PC,
        SRC_B_IMM);

    printf("jal x%d %d\n", *reg_rd_addr, *reg_imm);
}

void handle_jalr(
    uint32_t inst,
    uint32_t *regs,
    uint32_t *reg_rs1_val,
    uint32_t *reg_imm,
    uint8_t *reg_rd_addr,
    uint8_t *reg_ctrl_alu_op,
    uint8_t *reg_ctrl_alu_cmp,
    uint8_t *reg_ctrl_alu_src_a,
    uint8_t *reg_ctrl_alu_src_b)
{
    uint8_t rs1_addr = get_rs1_addr(inst);
    *reg_rs1_val = regs[rs1_addr];
    *reg_imm = get_imm_i(inst);
    *reg_rd_addr = get_rd_addr(inst);

    set_ctrl(
        reg_ctrl_alu_op,
        reg_ctrl_alu_cmp,
        reg_ctrl_alu_src_a,
        reg_ctrl_alu_src_b,
        ALU_OP_ADD,
        ALU_CMP_UNCONDITIONAL,
        SRC_A_RS1,
        SRC_B_IMM);

    printf("jalr x%d x%d %d\n", *reg_rd_addr, rs1_addr, *reg_imm);
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
            decode_unit->reg_rs1_val,
            decode_unit->reg_imm,
            decode_unit->reg_rd_addr,
            decode_unit->reg_ctrl_alu_op,
            decode_unit->reg_ctrl_alu_cmp,
            decode_unit->reg_ctrl_alu_src_a,
            decode_unit->reg_ctrl_alu_src_b);
        break;
    case OPCODE_LUI:
        handle_lui(
            inst,
            decode_unit->reg_imm,
            decode_unit->reg_rd_addr,
            decode_unit->reg_ctrl_alu_op,
            decode_unit->reg_ctrl_alu_cmp,
            decode_unit->reg_ctrl_alu_src_a,
            decode_unit->reg_ctrl_alu_src_b);
        break;
    case OPCODE_AUIPC:
        handle_auipc(
            inst,
            decode_unit->reg_imm,
            decode_unit->reg_rd_addr,
            decode_unit->reg_ctrl_alu_op,
            decode_unit->reg_ctrl_alu_cmp,
            decode_unit->reg_ctrl_alu_src_a,
            decode_unit->reg_ctrl_alu_src_b);
        break;
    case OPCODE_OP:
        handle_op(
            inst,
            decode_unit->regs,
            decode_unit->reg_rs1_val,
            decode_unit->reg_rs2_val,
            decode_unit->reg_rd_addr,
            decode_unit->reg_ctrl_alu_op,
            decode_unit->reg_ctrl_alu_cmp,
            decode_unit->reg_ctrl_alu_src_a,
            decode_unit->reg_ctrl_alu_src_b);
        break;
    case OPCODE_JAL:
        handle_jal(
            inst,
            decode_unit->reg_imm,
            decode_unit->reg_rd_addr,
            decode_unit->reg_ctrl_alu_op,
            decode_unit->reg_ctrl_alu_cmp,
            decode_unit->reg_ctrl_alu_src_a,
            decode_unit->reg_ctrl_alu_src_b);
        break;
    case OPCODE_JALR:
        handle_jalr(
            inst,
            decode_unit->regs,
            decode_unit->reg_rs1_val,
            decode_unit->reg_imm,
            decode_unit->reg_rd_addr,
            decode_unit->reg_ctrl_alu_op,
            decode_unit->reg_ctrl_alu_cmp,
            decode_unit->reg_ctrl_alu_src_a,
            decode_unit->reg_ctrl_alu_src_b);
        break;
    case OPCODE_BRANCH:
        break;
    case OPCODE_LOAD:
        break;
    case OPCODE_STORE:
        break;
    default:
        fprintf(stderr, "Error: Unknown opcode: %08x", opcode);
        exit(EXIT_FAILURE);
    }

    uint32_t funct3;
    uint32_t funct7;

    switch (opcode)
    {
    case OPCODE_OP_IMM:
        break;
    case OPCODE_LUI:
        break;
    case OPCODE_AUIPC:
        break;
    case OPCODE_OP:
        break;
    case OPCODE_JAL:
        break;
    case OPCODE_JALR:
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
