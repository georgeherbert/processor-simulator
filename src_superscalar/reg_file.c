#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "reg_file.h"

struct reg_file *reg_file_init()
{
    struct reg_file *reg_file = malloc(sizeof(struct reg_file));
    if (reg_file == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for register file");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < NUM_REGS; i++)
    {
        reg_file->regs[i].value = 0; // All values are initially zero
        reg_file->regs[i].qi = 0;    // All values are not initially in a reservation station
    }

    return reg_file;
}

void reg_file_step(struct reg_file *reg_file)
{
}

bool reg_file_is_reg_available(struct reg_file *reg_file, uint32_t index)
{
    return reg_file->regs[index].qi == 0;
}

void reg_file_destroy(struct reg_file *reg_file)
{
    free(reg_file);
}
