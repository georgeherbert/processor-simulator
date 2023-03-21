#ifndef ALU_H
#define ALU_H

#include "res_stations.h"
#include "reg_file.h"
#include "com_data_bus.h"

struct alu_unit
{
    struct res_stations *alu_res_stations; // Pointer to ALU reservation station
    struct reg_file *reg_file;             // Pointer to register file
    struct com_data_bus *cdb;              // Pointer to common data bus
};

struct alu_unit *alu_init(
    struct res_stations *alu_res_stations,
    struct reg_file *reg_file,
    struct com_data_bus *cdb);               // Initialise alu unit
void alu_step(struct alu_unit *alu_unit);    // Step alu unit
void alu_destroy(struct alu_unit *alu_unit); // Free alu unit

#endif // ALU_H
