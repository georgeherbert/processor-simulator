#ifndef MEMORY_H
#define MEMORY_H

#include "res_station.h"
#include "main_memory.h"

struct memory_unit
{
    struct res_station *memory_res_station; // Pointer to memory reservation station
    struct main_memory *mm;                 // Pointer to main memory
    uint32_t *regs;                         // Pointer to register file
};

struct memory_unit *memory_init(
    struct res_station *memory_res_station,
    struct main_memory *mm,
    uint32_t *regs);                                  // Initialise memory unit
void memory_step(struct memory_unit *memory_unit);    // Step memory unit
void memory_destroy(struct memory_unit *memory_unit); // Free memory unit

#endif // MEMORY_H
