#ifndef MEMORY_H
#define MEMORY_H

#include "res_stations.h"
#include "reg_file.h"
#include "main_memory.h"
#include "com_data_bus.h"

struct memory_unit
{
    struct res_stations *memory_res_stations; // Pointer to memory reservation station
    struct main_memory *mm;                   // Pointer to main memory
    struct reg_file *reg_file;                // Pointer to register file
    struct com_data_bus *cdb;                 // Pointer to common data bus
};

struct memory_unit *memory_init(
    struct res_stations *memory_res_stations,
    struct main_memory *mm,
    struct reg_file *reg_file,
    struct com_data_bus *cdb);                        // Initialise memory unit
void memory_step(struct memory_unit *memory_unit);    // Step memory unit
void memory_destroy(struct memory_unit *memory_unit); // Free memory unit

#endif // MEMORY_H
