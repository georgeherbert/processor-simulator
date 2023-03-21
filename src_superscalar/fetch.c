#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "fetch.h"
#include "main_memory.h"
#include "control.h"
#include "inst_queue.h"

struct fetch_unit *fetch_init(
    struct main_memory *mm,
    enum pc_src *pc_src,
    enum branch_in_pipeline *branch_in_pipeline,
    struct inst_queue *inst_queue,
    uint32_t *reg_pc_target,
    uint32_t *reg_inst,
    uint32_t *reg_pc,
    uint32_t *reg_npc)
{
    struct fetch_unit *fetch_unit = malloc(sizeof(struct fetch_unit));
    if (fetch_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for fetch unit\n");
        exit(EXIT_FAILURE);
    }

    fetch_unit->mm = mm;
    fetch_unit->pc_src = pc_src;
    fetch_unit->branch_in_pipeline = branch_in_pipeline;
    fetch_unit->inst_queue = inst_queue;
    fetch_unit->reg_pc_target = reg_pc_target;
    fetch_unit->reg_inst = reg_inst;
    fetch_unit->reg_pc = reg_pc;
    fetch_unit->reg_npc = reg_npc;

    return fetch_unit;
}

void fetch_step(struct fetch_unit *fetch_unit)
{
    if (inst_queue_is_not_full(fetch_unit->inst_queue) && *fetch_unit->branch_in_pipeline == BRANCH_NOT_IN_PIPELINE)
    {
        if (*fetch_unit->pc_src == PC_SRC_PLUS_4)
        {
            *fetch_unit->reg_pc = *fetch_unit->reg_npc;
        }
        else if (*fetch_unit->pc_src == PC_SRC_BRANCH)
        {
            *fetch_unit->reg_pc = *fetch_unit->reg_pc_target;
            *fetch_unit->pc_src = PC_SRC_PLUS_4;
        }
        else
        {
            fprintf(stderr, "Error: Invalid PC source control signal %d\n", *fetch_unit->pc_src);
            exit(EXIT_FAILURE);
        }
        uint32_t inst = main_memory_load_word(fetch_unit->mm, *fetch_unit->reg_pc);

        // TODO: Remove this. Eventually use speculative execution.
        if ((inst & 0x7F) == 0x6f || (inst & 0x7F) == 0x67 || (inst & 0x7F) == 0x63)
        {
            *fetch_unit->branch_in_pipeline = BRANCH_IN_PIPELINE;
        }

        *fetch_unit->reg_inst = inst;
        *fetch_unit->reg_npc = *fetch_unit->reg_pc + 4;
    }
}

void fetch_destroy(struct fetch_unit *fetch_unit)
{
    free(fetch_unit);
}
