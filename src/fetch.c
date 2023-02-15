#include "fetch.h"

struct fetch_unit fetch_init(struct memory *memory, uint32_t *reg_pc, uint32_t *reg_npc, uint32_t *reg_inst)
{
    struct fetch_unit fetch_unit =
        {
            .memory = memory,
            .reg_pc = reg_pc,
            .reg_npc = reg_npc,
            .reg_inst = reg_inst,
        };
    return fetch_unit;
}

void fetch_step(struct fetch_unit *fetch_unit)
{
    *fetch_unit->reg_inst = memory_load_word(fetch_unit->memory, *fetch_unit->reg_pc);
    *fetch_unit->reg_npc = *fetch_unit->reg_pc + 4;
}