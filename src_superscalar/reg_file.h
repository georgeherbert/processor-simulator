#ifndef REG_FILE_H
#define REG_FILE_H

#include <stdint.h>
#include <stdbool.h>
#include "com_data_bus.h"

#define NUM_REGS 32

struct reg
{
    uint32_t value; // Value of register (if not in reservation station)
    uint32_t qi;    // Reservation station ID
};

struct reg_file
{
    struct reg regs[NUM_REGS];
    struct com_data_bus *cdb; // Pointer to common data bus
};

struct reg_file *reg_file_init(struct com_data_bus *cdb);                            // Initialise register file
void reg_file_step(struct reg_file *reg_file);                                       // Step register file
uint32_t reg_file_get_reg_value_or_na(struct reg_file *reg_file, uint32_t reg_addr); // Get value of register or NA if in reservation station
uint32_t reg_file_get_reg_qi(struct reg_file *reg_file, uint32_t reg_addr);          // Get reservation station ID of register
void reg_file_set_reg_qi(struct reg_file *reg_file, uint32_t reg_addr, uint32_t qi); // Set reservation station ID of register
void reg_file_destroy(struct reg_file *reg_file);                                    // Free register file

#endif // REG_FILE_H
