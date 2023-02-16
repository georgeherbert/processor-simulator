#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "memory.h"
#include "fetch.h"
#include "decode.h"
#include "execute.h"

struct cpu *cpu_init(char *file_name)
{
    struct cpu *cpu = malloc(sizeof(struct cpu));
    if (cpu == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for cpu\n");
        exit(EXIT_FAILURE);
    }

    cpu->memory = memory_init(file_name);
    cpu->fetch_unit = fetch_init(
        cpu->memory,
        &cpu->reg_cond,
        &cpu->reg_alu_out,
        &cpu->reg_inst,
        &cpu->reg_pc);
    cpu->decode_unit = decode_init(
        cpu->regs,
        &cpu->reg_inst,
        &cpu->reg_rs1_val,
        &cpu->reg_rs2_val,
        &cpu->reg_imm,
        &cpu->reg_rd_addr,
        &cpu->reg_ctrl_alu_op,
        &cpu->reg_ctrl_alu_cmp,
        &cpu->reg_ctrl_alu_src_a,
        &cpu->reg_ctrl_alu_src_b);
    cpu->execute_unit = execute_init();

    cpu->reg_cond = 0;
    cpu->reg_alu_out = 0;
    cpu->reg_pc = -4;
    cpu->reg_inst = 0;

    cpu->reg_rs1_val = 0;
    cpu->reg_rs2_val = 0;
    cpu->reg_imm = 0;

    cpu->reg_cond = 0;

    printf("CPU successfully initialised\n");

    return cpu;
}

void cpu_destroy(struct cpu *cpu)
{
    memory_destroy(cpu->memory);
    fetch_destroy(cpu->fetch_unit);
    decode_destroy(cpu->decode_unit);
    execute_destroy(cpu->execute_unit);

    free(cpu);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct cpu *cpu = cpu_init(argv[1]);

    while (cpu->reg_pc < 80 || cpu->reg_pc == -4) // TODO: Fix this
    {
        // printf("Fetch %d %p \n", cpu->reg_pc, &cpu->reg_pc);
        fetch_step(cpu->fetch_unit);
        decode_step(cpu->decode_unit);
    }

    cpu_destroy(cpu);
    return 0;
}
