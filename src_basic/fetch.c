#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "fetch.h"
#include "main_memory.h"
#include "control.h"

struct fetch_unit *fetch_init(
    struct main_memory *mm,
    enum ctrl_pc_src *ctrl_pc_src,
    uint32_t *reg_alu_out,
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
    fetch_unit->ctrl_pc_src = ctrl_pc_src;
    fetch_unit->reg_alu_out = reg_alu_out;
    fetch_unit->reg_inst = reg_inst;
    fetch_unit->reg_pc = reg_pc;
    fetch_unit->reg_npc = reg_npc;

    return fetch_unit;
}

void fetch_step(struct fetch_unit *fetch_unit)
{
    if (*fetch_unit->ctrl_pc_src == CTRL_PC_SRC_NPC)
    {
        *fetch_unit->reg_pc = *fetch_unit->reg_npc;
    }
    else if (*fetch_unit->ctrl_pc_src == CTRL_PC_SRC_ALU_OUT)
    {
        *fetch_unit->reg_pc = *fetch_unit->reg_alu_out;
    }
    else
    {
        fprintf(stderr, "Error: Invalid PC source control signal\n");
        exit(EXIT_FAILURE);
    }
    *fetch_unit->reg_inst = main_memory_load_word(fetch_unit->mm, *fetch_unit->reg_pc);
    *fetch_unit->reg_npc = *fetch_unit->reg_pc + 4;
}

void fetch_destroy(struct fetch_unit *fetch_unit)
{
    free(fetch_unit);
}
