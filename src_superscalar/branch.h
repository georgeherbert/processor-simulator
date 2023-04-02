#ifndef BRANCH_H
#define BRANCH_H

#include <stdint.h>
#include "res_stations.h"
#include "reg_file.h"
#include "control.h"
#include "cdb.h"
#include "rob.h"

struct branch_unit
{
    struct res_stations *branch_res_stations; // Pointer to branch reservation station
    struct reg_file *reg_file;                // Pointer to register file
    struct cdb *cdb;                          // Pointer to common data bus

    struct rob *rob; // Pointer to reorder buffer
};

struct branch_unit *branch_init(
    struct res_stations *branch_res_stations,
    struct reg_file *reg_file,
    struct cdb *cdb,
    struct rob *rob);                                 // Initialise branch unit
void branch_step(struct branch_unit *branch_unit);    // Step branch unit
void branch_destroy(struct branch_unit *branch_unit); // Free branch unit

#endif // BRANCH_H
