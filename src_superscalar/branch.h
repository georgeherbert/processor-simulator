#ifndef BRANCH_H
#define BRANCH_H

#include <stdint.h>
#include "res_stations.h"
#include "reg_file.h"
#include "control.h"
#include "com_data_bus.h"

struct branch_unit
{
    struct res_stations *branch_res_stations;    // Pointer to branch reservation station
    struct reg_file *reg_file;                   // Pointer to register file
    uint32_t *reg_pc_target;                     // Pointer to PC target register
    enum pc_src *pc_src;                         // Pointer to control signal for PC source
    struct com_data_bus *cdb;                    // Pointer to common data bus
    enum branch_in_pipeline *branch_in_pipeline; // Control signal for presence of branch in pipeline
};

struct branch_unit *branch_init(
    struct res_stations *branch_res_stations,
    struct reg_file *reg_file,
    uint32_t *reg_pc_target,
    enum pc_src *pc_src,
    struct com_data_bus *cdb,
    enum branch_in_pipeline *branch_in_pipeline);     // Initialise branch unit
void branch_step(struct branch_unit *branch_unit);    // Step branch unit
void branch_destroy(struct branch_unit *branch_unit); // Free branch unit

#endif // BRANCH_H
