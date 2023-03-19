#ifndef REG_FILE_H
#define REG_FILE_H

#include <stdint.h>

#define NUM_REGS 32

struct reg
{
    uint32_t value; // Value of register (if not in reservation station)
    uint32_t qi;    // Reservation station ID
};

struct reg_file
{
    struct reg regs[NUM_REGS];
};

struct reg_file *reg_file_init();                 // Initialise register file
void reg_file_step(struct reg_file *reg_file);    // Step register file
void reg_file_destroy(struct reg_file *reg_file); // Free register file

#endif // REG_FILE_H