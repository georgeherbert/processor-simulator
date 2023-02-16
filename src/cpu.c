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
    cpu->fetch_unit = fetch_init(cpu->memory, &cpu->reg_pc, &cpu->reg_npc, &cpu->reg_inst);
    cpu->decode_unit = decode_init(cpu->memory, &cpu->reg_inst, &cpu->reg_rs1, &cpu->reg_rs2, &cpu->reg_imm);
    cpu->execute_unit = execute_init();

    cpu->reg_pc = 0;
    cpu->reg_npc = 0;
    cpu->reg_inst = 0;
    cpu->reg_rs1 = 0;
    cpu->reg_rs2 = 0;
    cpu->reg_imm = 0;

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

    while (cpu->reg_pc < 80)
    {
        // printf("Fetch %d %p \n", cpu->reg_pc, &cpu->reg_pc);
        fetch_step(cpu->fetch_unit);
        decode_step(cpu->decode_unit);
        // printf("Decode\n");
        cpu->reg_pc += 4;
    }

    cpu_destroy(cpu);
    return 0;
}
