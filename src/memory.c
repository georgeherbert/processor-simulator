#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "memory.h"
#include "main_memory.h"

struct memory_unit *memory_init(struct main_memory *mm, enum ctrl_mem *ctrl_mem, uint32_t *reg_alu_out, uint32_t *reg_rs2_val, uint32_t *reg_mdr)
{
    struct memory_unit *memory_unit = malloc(sizeof(struct memory_unit));
    if (memory_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for memory unit\n");
        exit(EXIT_FAILURE);
    }

    memory_unit->mm = mm;
    memory_unit->ctrl_mem = ctrl_mem;
    memory_unit->reg_alu_out = reg_alu_out;
    memory_unit->reg_rs2_val = reg_rs2_val;
    memory_unit->reg_mdr = reg_mdr;

    return memory_unit;
}

void memory_step(struct memory_unit *memory_unit)
{
    switch (*memory_unit->ctrl_mem)
    {
    case CTRL_MEM_NONE:
        break;
    case CTRL_MEM_LOAD_WORD:
        *memory_unit->reg_mdr = main_memory_load_word(memory_unit->mm, *memory_unit->reg_alu_out);
        break;
    case CTRL_MEM_LOAD_HALF:
        *memory_unit->reg_mdr = (int32_t)(int16_t)main_memory_load_half(memory_unit->mm, *memory_unit->reg_alu_out);
        break;
    case CTRL_MEM_LOAD_HALF_UNSIGNED:
        *memory_unit->reg_mdr = main_memory_load_half(memory_unit->mm, *memory_unit->reg_alu_out);
        break;
    case CTRL_MEM_LOAD_BYTE:
        *memory_unit->reg_mdr = (int32_t)(int8_t)main_memory_load_byte(memory_unit->mm, *memory_unit->reg_alu_out);
        break;
    case CTRL_MEM_LOAD_BYTE_UNSIGNED:
        *memory_unit->reg_mdr = main_memory_load_byte(memory_unit->mm, *memory_unit->reg_alu_out);
        break;
    case CTRL_MEM_STORE_WORD:
        printf("Storing word %d to address %d\n", *memory_unit->reg_rs2_val, *memory_unit->reg_alu_out);
        main_memory_store_word(memory_unit->mm, *memory_unit->reg_alu_out, *memory_unit->reg_rs2_val);
        break;
    case CTRL_MEM_STORE_HALF:
        main_memory_store_half(memory_unit->mm, *memory_unit->reg_alu_out, *memory_unit->reg_rs2_val);
        break;
    case CTRL_MEM_STORE_BYTE:
        main_memory_store_byte(memory_unit->mm, *memory_unit->reg_alu_out, *memory_unit->reg_rs2_val);
        break;
    default:
        fprintf(stderr, "Error: Invalid memory control signal\n");
        exit(EXIT_FAILURE);
    }
}

void memory_destroy(struct memory_unit *memory_unit)
{
    free(memory_unit);
}
