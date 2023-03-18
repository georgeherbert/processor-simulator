#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "cpu.h"
#include "main_memory.h"
#include "fetch.h"
#include "decode.h"
#include "inst_queue.h"
#include "memory.h"
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

    cpu->pc_src = PC_SRC_PLUS_4;

    cpu->reg_pc = 0;
    cpu->reg_npc = 0;

    cpu->mm = main_memory_init(file_name);
    cpu->fetch_unit = fetch_init(
        cpu->mm,
        &cpu->pc_src,
        &cpu->reg_pc_target,
        &cpu->reg_inst,
        &cpu->reg_pc,
        &cpu->reg_npc);
    cpu->inst_queue = inst_queue_init();
    cpu->decode_unit = decode_init(
        &cpu->reg_inst,
        &cpu->reg_pc,
        cpu->inst_queue);
    cpu->alu_res_station = res_station_init();
    cpu->branch_res_station = res_station_init();
    cpu->mem_res_station = res_station_init();
    cpu->issue_unit = issue_init(
        cpu->inst_queue,
        cpu->regs,
        cpu->alu_res_station,
        cpu->branch_res_station,
        cpu->mem_res_station);
    cpu->alu_unit = alu_init(
        cpu->alu_res_station,
        cpu->regs);
    cpu->branch_unit = branch_init(
        cpu->branch_res_station,
        cpu->regs,
        &cpu->reg_pc_target,
        &cpu->pc_src);
    cpu->memory_unit = memory_init(
        cpu->mem_res_station,
        cpu->mm,
        cpu->regs);

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
}

void cpu_destroy(struct cpu *cpu)
{
    main_memory_destroy(cpu->mm);
    fetch_destroy(cpu->fetch_unit);
    decode_destroy(cpu->decode_unit);
    inst_queue_destroy(cpu->inst_queue);
    res_station_destroy(cpu->alu_res_station);
    res_station_destroy(cpu->mem_res_station);
    res_station_destroy(cpu->branch_res_station);
    issue_destroy(cpu->issue_unit);
    alu_destroy(cpu->alu_unit);
    branch_destroy(cpu->branch_unit);
    memory_destroy(cpu->memory_unit);

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
    while (!(cpu->pc_src == PC_SRC_BRANCH && cpu->reg_pc_target == 0))
    {
        fetch_step(cpu->fetch_unit);
        decode_step(cpu->decode_unit);
        issue_step(cpu->issue_unit);
        alu_step(cpu->alu_unit);
        branch_step(cpu->branch_unit);
        memory_step(cpu->memory_unit);
        // execute_step(cpu->execute_unit);
        // memory_step(cpu->memory_unit);
        // writeback_step(cpu->writeback_unit);

        instructions++;
        cycles += 5;
        // print_regs(cpu->regs);
    }

    print_main_memory(cpu->mm);
    print_regs(cpu->regs);

    printf("\nInstructions: %" PRIu64 "\n", instructions);
    printf("Cycles: %" PRIu64 "\n", cycles);

    cpu_destroy(cpu);
    return 0;
}
