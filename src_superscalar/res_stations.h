#ifndef RES_STATIONS_H
#define RES_STATIONS_H

#include <stdint.h>
#include <stdbool.h>
#include "decode.h"

#define RES_STATION_SIZE 32

struct res_station
{
    uint32_t id;
    bool busy;
    enum op op;
    uint32_t qj;
    uint32_t qk;
    uint32_t vj;
    uint32_t vk;
    uint32_t a;
    uint32_t inst_pc;
    uint32_t dest; // TODO: Remove this eventually
};

struct res_stations
{
    struct res_station *stations;
    uint32_t num_stations;
};

struct res_stations *res_stations_init(
    uint32_t num_stations,
    uint32_t id_offset);                         // Initialise reservation stations
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
    uint32_t inst_pc);                                           // Add instruction to reservation stations
struct res_station res_stations_remove(struct res_stations *rs); // Remove instruction from reservation stations
bool res_stations_not_empty(struct res_stations *rs);            // Check if reservation stations are not empty
bool res_stations_not_full(struct res_stations *rs);             // Check if reservation stations are not full
// TODO: The res_stations_add args need to be fixed

void res_stations_destroy(struct res_stations *rs); // Free reservation stations

#endif // RES_STATIONS_H
