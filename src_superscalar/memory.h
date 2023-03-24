#ifndef MEMORY_H
#define MEMORY_H

#include "reg_file.h"
#include "main_memory.h"
#include "com_data_bus.h"
#include "reg.h"
#include "memory_buffers.h"

struct memory_unit
{
    struct memory_buffers *memory_buffers; // Pointer to memory buffers
    struct main_memory *mm;                // Pointer to main memory
    struct reg_file *reg_file;             // Pointer to register file
    struct com_data_bus *cdb;              // Pointer to common data bus
    struct reg *memory_buffers_ready;      // Pointer to indicate if a memory buffer is ready
};

struct memory_unit *memory_init(
    struct memory_buffers *memory_buffers,
    struct main_memory *mm,
    struct reg_file *reg_file,
    struct com_data_bus *cdb,
    struct reg *memory_buffers_ready);                // Initialise memory unit
void memory_step(struct memory_unit *memory_unit);    // Step memory unit
void memory_destroy(struct memory_unit *memory_unit); // Free memory unit

#endif // MEMORY_H
