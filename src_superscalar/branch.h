#ifndef BRANCH_H
#define BRANCH_H

#include <stdint.h>
#include "res_station.h"
#include "control.h"

struct branch_unit
{
    struct res_station *branch_res_station; // Pointer to branch reservation station
    uint32_t *regs;                         // Pointer to register file
    uint32_t *reg_pc_target;                // Pointer to PC target register
    enum pc_src *pc_src;                    // Pointer to control signal for PC source
};

struct branch_unit *branch_init(
    struct res_station *branch_res_station,
    uint32_t *regs,
    uint32_t *reg_pc_target,
    enum pc_src *pc_src);                             // Initialise branch unit
void branch_step(struct branch_unit *branch_unit);    // Step branch unit
void branch_destroy(struct branch_unit *branch_unit); // Free branch unit

#endif // BRANCH_H
