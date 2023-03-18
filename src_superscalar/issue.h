#ifndef ISSUE_H
#define ISSUE_H

#include <stdint.h>
#include "inst_queue.h"
#include "res_station.h"

struct issue_unit
{
    struct inst_queue *inst_queue;          // Pointer to instruction queue
    uint32_t *regs;                         // Pointer to register file
    struct res_station *alu_res_station;    // Pointer to ALU reservation station
    struct res_station *mem_res_station;    // Pointer to memory reservation station
    struct res_station *branch_res_station; // Pointer to branch reservation station
};

struct issue_unit *issue_init(
    struct inst_queue *inst_queue,
    uint32_t *regs,
    struct res_station *alu_res_station,
    struct res_station *branch_res_station,
    struct res_station *mem_res_station);          // Initialise issue unit
void issue_step(struct issue_unit *issue_unit);    // Step issue unit
void issue_destroy(struct issue_unit *issue_unit); // Free issue unit

#endif // ISSUE_H
