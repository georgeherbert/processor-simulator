#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "fetch.h"
#include "main_memory.h"
#include "control.h"
#include "inst_queue.h"
#include "reg.h"

struct fetch_unit *fetch_init(
    struct main_memory *mm,
    struct reg *pc_src,
    struct inst_queue *inst_queue,
    struct reg *reg_pc_target,
    struct reg *reg_inst,
    struct reg *reg_inst_pc)
{
    struct fetch_unit *fetch_unit = malloc(sizeof(struct fetch_unit));
    if (fetch_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for fetch unit\n");
        exit(EXIT_FAILURE);
    }

    fetch_unit->mm = mm;
    fetch_unit->pc_src = pc_src;
    fetch_unit->inst_queue = inst_queue;
    fetch_unit->reg_pc_target = reg_pc_target;
    fetch_unit->reg_inst = reg_inst;
    fetch_unit->reg_inst_pc = reg_inst_pc;
    fetch_unit->reg_pc = 0;
    fetch_unit->reg_npc = 0;

    return fetch_unit;
}

void fetch_step(struct fetch_unit *fetch_unit)
{
    if (!inst_queue_full(fetch_unit->inst_queue))
    {
        if (reg_read(fetch_unit->pc_src) == PC_SRC_NORMAL)
        {
            fetch_unit->reg_pc = fetch_unit->reg_npc;
        }
        else if (reg_read(fetch_unit->pc_src) == PC_SRC_MISPREDICT)
        {
            fetch_unit->reg_pc = reg_read(fetch_unit->reg_pc_target);
            reg_write(fetch_unit->pc_src, PC_SRC_NORMAL);
        }
        else
        {
            fprintf(stderr, "Error: Invalid PC source control signal %d\n", reg_read(fetch_unit->pc_src));
            exit(EXIT_FAILURE);
        }
        uint32_t inst = main_memory_load_word(fetch_unit->mm, fetch_unit->reg_pc);

        // TODO: Remove this. Eventually use speculative execution.
        if ((inst & 0x7F) == 0x6f || (inst & 0x7F) == 0x67 || (inst & 0x7F) == 0x63)
        {
            // reg_write(fetch_unit->branch_in_pipeline, BRANCH_IN_PIPELINE);
        }

        reg_write(fetch_unit->reg_inst, inst);
        reg_write(fetch_unit->reg_inst_pc, fetch_unit->reg_pc);
        fetch_unit->reg_npc = fetch_unit->reg_pc + 4;
    }
}

void fetch_destroy(struct fetch_unit *fetch_unit)
{
    free(fetch_unit);
}
