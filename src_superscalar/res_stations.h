#ifndef RES_STATIONS_H
#define RES_STATIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "decode.h"
#include "reg_file.h"
#include "cdb.h"
#include "reg.h"

struct res_station
{
    uint32_t id;      // ID of reservation station
    bool busy;        // Whether reservation station is busy
    enum op op;       // Operation to perform
    uint32_t qj;      // Reservation station ID of first operand
    uint32_t qk;      // Reservation station ID of second operand
    uint32_t vj;      // val of first operand
    uint32_t vk;      // val of second operand
    uint32_t a;       // Immediate val or address
    uint32_t rob_id;  // Destination ROB ID
    uint32_t inst_pc; // Program counter of instruction
};

struct res_stations
{
    struct res_station *stations_current; // Array of current reservation stations
    struct res_station *stations_next;    // Array of next reservation stations
    struct reg_file *reg_file;            // Pointer to register file
    uint32_t num_stations;                // Number of reservation stations
    struct cdb *cdb;                      // Pointer to common data bus
};

struct res_stations *res_stations_init(
    uint32_t num_stations,
    uint32_t id_offset,
    struct reg_file *reg_file,
    struct cdb *cdb);                            // Initialise reservation stations
void res_stations_step(struct res_stations *rs); // Step reservation stations
void res_stations_add(
    struct res_stations *rs,
    enum op op,
    uint32_t qj,
    uint32_t qk,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t dest,
    uint32_t inst_pc);                                                        // Add instruction to reservation stations
bool res_stations_all_busy(struct res_stations *rs);                          // Check if all reservation stations are busy
struct res_station *res_stations_remove(struct res_stations *rs, uint8_t id); // Remove instruction from reservation stations
void res_stations_clear(struct res_stations *rs);                             // Clear reservation stations on mispredict
void res_stations_update_current(struct res_stations *rs);                    // Update current reservation stations
void res_stations_destroy(struct res_stations *rs);                           // Free reservation stations

#endif // RES_STATIONS_H
