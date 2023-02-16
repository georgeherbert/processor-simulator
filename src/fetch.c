#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "fetch.h"
#include "memory.h"

struct fetch_unit *fetch_init(struct memory *memory, uint8_t *reg_cond, uint32_t *reg_alu_out, uint32_t *reg_inst, uint32_t *reg_pc)
{
    struct fetch_unit *fetch_unit = malloc(sizeof(struct fetch_unit));
    if (fetch_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for fetch unit\n");
        exit(EXIT_FAILURE);
    }

    fetch_unit->memory = memory;
    fetch_unit->reg_cond = reg_cond;
    fetch_unit->reg_alu_out = reg_alu_out;
    fetch_unit->reg_inst = reg_inst;
    fetch_unit->reg_pc = reg_pc;

    return fetch_unit;
}

void fetch_step(struct fetch_unit *fetch_unit)
{
    *fetch_unit->reg_pc = *fetch_unit->reg_cond ? *fetch_unit->reg_alu_out : *fetch_unit->reg_pc + 4;
    *fetch_unit->reg_inst = memory_load_word(fetch_unit->memory, *fetch_unit->reg_pc);
}

void fetch_destroy(struct fetch_unit *fetch_unit)
{
    free(fetch_unit);
}
