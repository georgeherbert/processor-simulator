#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "reg_file.h"
#include "cpu.h"

#define NA 0

struct reg_file *reg_file_init(struct com_data_bus *cdb)
{
    struct reg_file *reg_file = malloc(sizeof(struct reg_file));
    if (reg_file == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for register file");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < NUM_REGS; i++)
    {
        reg_file->regs[i].val = 0; // All vals are initially zero
        reg_file->regs[i].qi = 0;  // All vals are not initially in a reservation station
    }

    reg_file->regs[8].val = MEMORY_SIZE;
    reg_file->regs[2].val = MEMORY_SIZE;

    reg_file->cdb = cdb;

    return reg_file;
}

uint32_t reg_file_get_reg_val_or_na(struct reg_file *reg_file, uint32_t reg_addr)
{
    /*
        If the register is awaiting a val from a reservation station we return NA (i.e. zero).
        But, we could in theory return any arbitrary val since it won't be used regardless.
    */
    return reg_file->regs[reg_addr].qi == 0 ? reg_file->regs[reg_addr].val : NA;
}

uint32_t reg_file_get_reg_qi(struct reg_file *reg_file, uint32_t reg_addr)
{
    return reg_file->regs[reg_addr].qi;
}

void reg_file_step(struct reg_file *reg_file)
{
    for (uint32_t i = 0; i < NUM_REGS; i++)
    {
        if (reg_file->regs[i].qi != 0)
        {
            if (com_data_bus_is_val_ready(reg_file->cdb, reg_file->regs[i].qi))
            {
                reg_file->regs[i].val = com_data_bus_get_val(reg_file->cdb, reg_file->regs[i].qi);
                reg_file->regs[i].qi = 0;
            }
        }
    }
}

void reg_file_set_reg_qi(struct reg_file *reg_file, uint32_t reg_addr, uint32_t qi)
{
    // Register zero is always zero and cannot be written to
    if (reg_addr != 0)
    {
        reg_file->regs[reg_addr].qi = qi;
    }
}

void reg_file_destroy(struct reg_file *reg_file)
{
    free(reg_file);
}
