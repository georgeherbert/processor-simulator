#ifndef BRANCH_H
#define BRANCH_H

#include <stdint.h>
#include "res_stations.h"
#include "reg_file.h"
#include "control.h"
#include "cdb.h"
#include "reg.h"

struct branch_unit
{
    struct res_stations *branch_res_stations; // Pointer to branch reservation station
    struct reg_file *reg_file;                // Pointer to register file
    struct reg *reg_pc_target;                // Pointer to PC target register
    struct reg *pc_src;                       // Pointer to control signal for PC source
    struct cdb *cdb;                          // Pointer to common data bus
    struct reg *branch_in_pipeline;           // Control signal for presence of branch in pipeline
};

struct branch_unit *branch_init(
    struct res_stations *branch_res_stations,
    struct reg_file *reg_file,
    struct reg *reg_pc_target,
    struct reg *pc_src,
    struct cdb *cdb,
    struct reg *branch_in_pipeline);                  // Initialise branch unit
void branch_step(struct branch_unit *branch_unit);    // Step branch unit
void branch_destroy(struct branch_unit *branch_unit); // Free branch unit

#endif // BRANCH_H
