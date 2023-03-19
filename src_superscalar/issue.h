#ifndef ISSUE_H
#define ISSUE_H

#include <stdint.h>
#include "inst_queue.h"
#include "res_stations.h"
#include "reg_file.h"

struct issue_unit
{
    struct inst_queue *inst_queue;            // Pointer to instruction queue
    struct reg_file *reg_file;                // Pointer to register file
    struct res_stations *alu_res_stations;    // Pointer to ALU reservation station
    struct res_stations *memory_res_stations; // Pointer to memory reservation station
    struct res_stations *branch_res_stations; // Pointer to branch reservation station
};

struct issue_unit *issue_init(
    struct inst_queue *inst_queue,
    struct reg_file *reg_file,
    struct res_stations *alu_res_stations,
    struct res_stations *branch_res_stations,
    struct res_stations *memory_res_stations);     // Initialise issue unit
void issue_step(struct issue_unit *issue_unit);    // Step issue unit
void issue_destroy(struct issue_unit *issue_unit); // Free issue unit

#endif // ISSUE_H
