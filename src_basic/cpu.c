#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "cpu.h"
#include "main_memory.h"
#include "fetch.h"
#include "decode.h"
#include "execute.h"
#include "memory.h"
#include "writeback.h"
#include "control.h"

#define NUM_WORDS_OUTPUT 2048

struct cpu *cpu_init(char *file_name)
{
    struct cpu *cpu = malloc(sizeof(struct cpu));
    if (cpu == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for cpu\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < NUM_REGS; i++)
    {
        cpu->regs[i] = 0;
    }
    // Set frame pointer and stack pointer
    cpu->regs[8] = MEMORY_SIZE;
    cpu->regs[2] = MEMORY_SIZE;

    cpu->ctrl_pc_src = CTRL_PC_SRC_NPC;

    cpu->reg_pc = 0;
    cpu->reg_npc = 0;

    cpu->mm = main_memory_init(file_name);
    cpu->fetch_unit = fetch_init(
        cpu->mm,
        &cpu->ctrl_pc_src,
        &cpu->reg_alu_out,
        &cpu->reg_inst,
        &cpu->reg_pc,
        &cpu->reg_npc);
    cpu->decode_unit = decode_init(
        cpu->regs,
        &cpu->reg_inst,
        &cpu->reg_rd_addr,
        &cpu->reg_rs1_val,
        &cpu->reg_rs2_val,
        &cpu->reg_imm,
        &cpu->ctrl_src_a,
        &cpu->ctrl_src_b,
        &cpu->ctrl_alu_op,
        &cpu->ctrl_cmp,
        &cpu->ctrl_mem,
        &cpu->ctrl_wb);
    cpu->execute_unit = execute_init(
        &cpu->reg_pc,
        &cpu->reg_rs1_val,
        &cpu->reg_rs2_val,
        &cpu->reg_imm,
        &cpu->ctrl_src_a,
        &cpu->ctrl_src_b,
        &cpu->ctrl_alu_op,
        &cpu->ctrl_cmp,
        &cpu->reg_alu_out,
        &cpu->ctrl_pc_src);
    cpu->memory_unit = memory_init(
        cpu->mm,
        &cpu->ctrl_mem,
        &cpu->reg_alu_out,
        &cpu->reg_rs2_val,
        &cpu->reg_mdr);
    cpu->writeback_unit = writeback_init(
        cpu->regs,
        &cpu->ctrl_wb,
        &cpu->reg_rd_addr,
        &cpu->reg_alu_out,
        &cpu->reg_mdr,
        &cpu->reg_npc);

    printf("CPU successfully initialised\n");

    return cpu;
}

void print_main_memory(struct main_memory *mm)
{
    printf("\nHighest %d words of main memory:\n", NUM_WORDS_OUTPUT);
    uint32_t start_index = MEMORY_SIZE - NUM_WORDS_OUTPUT * 4;
    uint32_t initial_offset = start_index % 16;

    for (uint32_t i = start_index; i < MEMORY_SIZE; i += 4)
    {
        printf("%010u: %-11d ", i, main_memory_load_word(mm, i));
        if ((i + 4) % 16 == initial_offset)
        {
            printf("\n");
        }
    }
}

void print_regs(uint32_t *regs)
{
    printf("\nRegisters:\n");
    for (int i = 0; i < NUM_REGS; i++)
    {
        printf("x%02d: %-11d ", i, regs[i]);
        if ((i + 1) % 4 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

void cpu_destroy(struct cpu *cpu)
{
    main_memory_destroy(cpu->mm);
    fetch_destroy(cpu->fetch_unit);
    decode_destroy(cpu->decode_unit);
    execute_destroy(cpu->execute_unit);
    memory_destroy(cpu->memory_unit);
    writeback_destroy(cpu->writeback_unit);

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

    uint64_t instructions = 0;
    uint64_t cycles = 0;

    // Cycle until PC will be 0
    while (!(cpu->ctrl_pc_src == CTRL_PC_SRC_ALU_OUT && cpu->reg_alu_out == 0))
    {
        fetch_step(cpu->fetch_unit);
        // printf("PC: %d\n", cpu->reg_pc);
        decode_step(cpu->decode_unit);
        execute_step(cpu->execute_unit);
        memory_step(cpu->memory_unit);
        writeback_step(cpu->writeback_unit);

        instructions++;
        cycles += 5;
        // print_main_memory(cpu->mm);
        // print_regs(cpu->regs);
    }

    print_main_memory(cpu->mm);
    print_regs(cpu->regs);

    printf("\nInstructions: %" PRIu64 "\n", instructions);
    printf("Cycles: %" PRIu64 "\n", cycles);

    cpu_destroy(cpu);
    return 0;
}
