#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "cpu.h"
#include "main_memory.h"
#include "reg_file.h"
#include "fetch.h"
#include "decode.h"
#include "inst_queue.h"
#include "issue.h"
#include "res_stations.h"
#include "reg_file.h"
#include "alu.h"
#include "branch.h"
#include "memory.h"
#include "control.h"
#include "reg.h"

#define NUM_WORDS_OUTPUT 128

#define NUM_ALU_RES_STATIONS 32
#define NUM_BRANCH_RES_STATIONS 32
#define NUM_MEMORY_RES_STATIONS 1

struct cpu *cpu_init(char *file_name)
{
    struct cpu *cpu = malloc(sizeof(struct cpu));
    if (cpu == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for cpu\n");
        exit(EXIT_FAILURE);
    }

    cpu->cdb = com_data_bus_init(3); // One entry for each functional unit
    cpu->reg_file = reg_file_init(cpu->cdb);

    cpu->pc_src.val_current = PC_SRC_PLUS_4;
    cpu->branch_in_pipeline.val_current = BRANCH_NOT_IN_PIPELINE;
    cpu->reg_inst.val_current = 0x0;
    cpu->mm = main_memory_init(file_name);
    cpu->inst_queue = inst_queue_init();
    cpu->fetch_unit = fetch_init(
        cpu->mm,
        &cpu->pc_src,
        &cpu->branch_in_pipeline,
        cpu->inst_queue,
        &cpu->reg_pc_target,
        &cpu->reg_inst,
        &cpu->reg_inst_pc);
    cpu->decode_unit = decode_init(
        &cpu->reg_inst,
        &cpu->reg_inst_pc, // TODO: Remove this
        cpu->inst_queue);
    cpu->alu_res_stations = res_stations_init(
        NUM_ALU_RES_STATIONS,
        1, // 1 is used because 0 indicates operands are ready
        cpu->reg_file,
        cpu->cdb);
    cpu->branch_res_stations = res_stations_init(
        NUM_BRANCH_RES_STATIONS,
        1 + NUM_ALU_RES_STATIONS,
        cpu->reg_file,
        cpu->cdb);
    cpu->memory_res_stations = res_stations_init(
        NUM_MEMORY_RES_STATIONS,
        1 + NUM_ALU_RES_STATIONS + NUM_BRANCH_RES_STATIONS,
        cpu->reg_file,
        cpu->cdb);
    cpu->issue_unit = issue_init(
        cpu->inst_queue,
        cpu->reg_file,
        cpu->alu_res_stations,
        cpu->branch_res_stations,
        cpu->memory_res_stations);
    cpu->alu_unit = alu_init(
        cpu->alu_res_stations,
        cpu->reg_file,
        cpu->cdb);
    cpu->branch_unit = branch_init(
        cpu->branch_res_stations,
        cpu->reg_file,
        &cpu->reg_pc_target,
        &cpu->pc_src,
        cpu->cdb,
        &cpu->branch_in_pipeline);
    cpu->memory_unit = memory_init(
        cpu->memory_res_stations,
        cpu->mm,
        cpu->reg_file,
        cpu->cdb);

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

void print_reg_file(struct reg_file *reg_file)
{
    printf("\nRegisters:\n");
    for (int i = 0; i < NUM_REGS; i++)
    {
        printf("x%02d: ", i);
        if (reg_file->regs[i].qi == 0)
        {
            printf("%-11d ", reg_file->regs[i].val);
        }
        else
        {
            printf("#%-10d ", reg_file->regs[i].qi);
        }

        if ((i + 1) % 4 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}

void cpu_destroy(struct cpu *cpu)
{
    com_data_bus_destroy(cpu->cdb);
    reg_file_destroy(cpu->reg_file);
    main_memory_destroy(cpu->mm);
    fetch_destroy(cpu->fetch_unit);
    decode_destroy(cpu->decode_unit);
    inst_queue_destroy(cpu->inst_queue);
    issue_destroy(cpu->issue_unit);
    res_stations_destroy(cpu->alu_res_stations);
    res_stations_destroy(cpu->memory_res_stations);
    res_stations_destroy(cpu->branch_res_stations);
    alu_destroy(cpu->alu_unit);
    branch_destroy(cpu->branch_unit);
    memory_destroy(cpu->memory_unit);

    free(cpu);
}

void update_current(struct cpu *cpu)
{
    reg_update_current(&cpu->pc_src);
    reg_update_current(&cpu->branch_in_pipeline);
    reg_update_current(&cpu->reg_pc_target);
    reg_update_current(&cpu->reg_inst);
    reg_update_current(&cpu->reg_inst_pc);

    inst_queue_update_current(cpu->inst_queue);

    res_stations_update_current(cpu->alu_res_stations);
    res_stations_update_current(cpu->branch_res_stations);
    res_stations_update_current(cpu->memory_res_stations);
}

void step(struct cpu *cpu)
{
    if (!cpu->jump_to_zero)
    {
        fetch_step(cpu->fetch_unit);
        decode_step(cpu->decode_unit);
    }
    issue_step(cpu->issue_unit);
    alu_step(cpu->alu_unit);
    branch_step(cpu->branch_unit);
    memory_step(cpu->memory_unit);
    res_stations_step(cpu->alu_res_stations);
    res_stations_step(cpu->branch_res_stations);
    res_stations_step(cpu->memory_res_stations);
    reg_file_step(cpu->reg_file);
    com_data_bus_step(cpu->cdb);
}

bool ready_to_exit(struct cpu *cpu)
{
    // Is the last instruction a jump to zero?
    bool jump_to_zero = cpu->pc_src.val_current == PC_SRC_BRANCH && cpu->reg_pc_target.val_current == 0;
    cpu->jump_to_zero = cpu->jump_to_zero || jump_to_zero;

    bool all_regs_written = true;
    for (int i = 0; i < NUM_REGS; i++)
    {
        all_regs_written &= cpu->reg_file->regs[i].qi == 0;
    }

    // If we have had a jump to zero and all registers have been written, the program we exit
    bool ready_to_exit = cpu->jump_to_zero && all_regs_written;

    return ready_to_exit;
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
    while (!ready_to_exit(cpu))
    {
        // printf("\nCycle: %" PRIu64 "\n", cycles);

        step(cpu);
        update_current(cpu);

        // print_main_memory(cpu->mm);
        // print_reg_file(cpu->reg_file);

        instructions++;
        cycles++;
    }

    print_main_memory(cpu->mm);
    print_reg_file(cpu->reg_file);

    printf("\nInstructions: %" PRIu64 "\n", instructions);
    printf("Cycles: %" PRIu64 "\n", cycles);

    cpu_destroy(cpu);
    return 0;
}
