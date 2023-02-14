#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "rv32i.h"

#define NUM_REGS 32
#define DATA_MEM_SIZE 1024

rv32i_t *init_inst_mem(char *filename, uint32_t *inst_mem_size)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        perror("Error: could not open file.\n");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    *inst_mem_size = ftell(fp);
    rewind(fp);

    rv32i_t *inst_mem = malloc(*inst_mem_size * sizeof(uint8_t));
    uint32_t word_index = 0;

    rv32i_t inst;
    while (fread(&inst, sizeof(rv32i_t), 1, fp) == 1)
    {
        inst_mem[word_index] = inst;
        word_index++;
    }

    fclose(fp);

    return inst_mem;
}

// Sign extension
int32_t intn_t_to_int32_t(int32_t imm, uint8_t n)
{
    int32_t sign_bit = 1 << (n - 1);
    int32_t mask = (1 << n) - 1;
    return (imm & sign_bit) ? (imm | ~mask) : (imm & mask);
}

void fetch(uint32_t *pc, rv32i_t *inst_mem, rv32i_t *inst_reg, uint32_t *next_pc_reg)
{
    *inst_reg = inst_mem[*pc / 4];
    *next_pc_reg = *pc + 4;
}

Opcode decode(rv32i_t inst_reg, int32_t *regs, int32_t *rs1_reg, int32_t *rs2_reg, int32_t *imm_reg)
{
    Opcode opcode = inst_reg.i_type.opcode;
    switch (opcode)
    {
    case ADDI:
    case SLTI:
    case SLTIU:
    case ANDI:
    case ORI:
    case XORI:
    case JALR:
    case LW:
    case LH:
    case LHU:
    case LB:
    case LBU:
        *rs1_reg = regs[inst_reg.i_type.rs1];
        *imm_reg = intn_t_to_int32_t(inst_reg.i_type.imm, 12);
        break;
    case SLLI:
    case SRLI:
    case SRAI:
        *rs1_reg = regs[inst_reg.i_type_shift.rs1];
        *imm_reg = inst_reg.i_type_shift.shamt;
        break;
    case LUI:
    case AUIPC:
    case JAL:
        *imm_reg = intn_t_to_int32_t(inst_reg.u_type.imm, 20);
        break;
    case ADD:
    case SLT:
    case SLTU:
    case AND:
    case OR:
    case XOR:
    case SLL:
    case SRL:
    case SUB:
    case SRA:
        *rs1_reg = regs[inst_reg.r_type.rs1];
        *rs2_reg = regs[inst_reg.r_type.rs2];
        break;
    case BEQ:
    case BNE:
    case BLT:
    case BLTU:
    case BGE:
    case BGEU:
    case SW:
    case SH:
    case SB:
        *rs1_reg = regs[inst_reg.s_type.rs1];
        *rs2_reg = regs[inst_reg.s_type.rs2];
        *imm_reg = intn_t_to_int32_t(inst_reg.s_type.imm, 12);
        break;
    }

    return opcode;
}

void execute(Opcode opcode, int32_t rs1_reg, int32_t rs2_reg, int32_t imm_reg, uint32_t pc, uint32_t next_pc_reg, int32_t *alu_output_reg, uint8_t *cond_reg)
{
    switch (opcode)
    {
    case ADDI:
        *alu_output_reg = rs1_reg + imm_reg;
        break;
    case SLTI:
        *alu_output_reg = rs1_reg < imm_reg;
        break;
    case SLTIU:
        *alu_output_reg = (uint32_t)rs1_reg < (uint32_t)imm_reg;
        break;
    case ANDI:
        *alu_output_reg = rs1_reg & imm_reg;
        break;
    case ORI:
        *alu_output_reg = rs1_reg | imm_reg;
        break;
    case XORI:
        *alu_output_reg = rs1_reg ^ imm_reg;
        break;
    case SLLI:
        *alu_output_reg = rs1_reg << imm_reg;
        break;
    case SRLI:
        *alu_output_reg = (uint32_t)rs1_reg >> imm_reg;
        break;
    case SRAI:
        *alu_output_reg = rs1_reg >> imm_reg;
        break;
    case LUI:
        *alu_output_reg = imm_reg << 12;
        break;
    case AUIPC:
        *alu_output_reg = pc + (imm_reg << 12);
        break;

    case ADD:
        *alu_output_reg = rs1_reg + rs2_reg;
        break;
    case SLT:
        *alu_output_reg = rs1_reg < rs2_reg;
        break;
    case SLTU:
        *alu_output_reg = (uint32_t)rs1_reg < (uint32_t)rs2_reg;
        break;
    case AND:
        *alu_output_reg = rs1_reg & rs2_reg;
        break;
    case OR:
        *alu_output_reg = rs1_reg | rs2_reg;
        break;
    case XOR:
        *alu_output_reg = rs1_reg ^ rs2_reg;
        break;
    case SLL:
        *alu_output_reg = rs1_reg << rs2_reg;
        break;
    case SRL:
        *alu_output_reg = (uint32_t)rs1_reg >> rs2_reg;
        break;
    case SUB:
        *alu_output_reg = rs1_reg - rs2_reg;
        break;
    case SRA:
        *alu_output_reg = rs1_reg >> rs2_reg;
        break;

    case JAL:
        break;
    case JALR:
        break;

    case BEQ:
        *cond_reg = rs1_reg == rs2_reg;
        break;
    case BNE:
        break;
    case BLT:
        break;
    case BLTU:
        break;
    case BGE:
        break;
    case BGEU:
        break;

    case LW:
        break;
    case LH:
        break;
    case LHU:
        break;
    case LB:
        break;
    case LBU:
        break;
    case SW:
        break;
    case SH:
        break;
    case SB:
        break;
    }
    printf("\n");
}

void output_registers(uint32_t *registers)
{
    for (uint8_t i = 0; i < NUM_REGS; i++)
    {
        printf("x%d: %d\n", i, registers[i]);
    }
}

int main(int argc, char *argv[])
{
    int32_t regs[NUM_REGS] = {0};                               // Registers
    uint8_t data_mem[DATA_MEM_SIZE] = {0};                      // Data memory (byte-addressable)
    uint32_t inst_mem_size;                                     // Number of bytes in the instruction memory
    rv32i_t *inst_mem = init_inst_mem(argv[1], &inst_mem_size); // Instruction memory (word-addressable)
    uint32_t pc = 0;                                            // Program counter

    rv32i_t inst_reg;       // Instruction register
    uint32_t next_pc_reg;   // Next program counter
    int32_t rs1_reg;        // Temporary rs1
    int32_t rs2_reg;        // Temporary rs2
    int32_t imm_reg;        // Immediate value
    int32_t alu_output_reg; // ALU output
    uint8_t cond_reg;       // Condition
    int32_t ld_mem_reg;     // Load memory

    printf("Hello world how are you\n");
    printf("pc: %" PRIu32 "\n", pc);

    while (pc < inst_mem_size)
    {
        fetch(pc, inst_mem, &inst_reg, &next_pc_reg);
        // printf("fetch");
        Opcode opcode = decode(inst_reg, regs, &rs1_reg, &rs2_reg, &imm_reg);
        // printf("decode");
        execute(opcode, rs1_reg, rs2_reg, imm_reg, pc, next_pc_reg, &alu_output_reg, &cond_reg);
        // printf("execute");
        pc += 4;
    }

    // output_registers(regs);
    return 0;
}
