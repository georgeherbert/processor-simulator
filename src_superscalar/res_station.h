#ifndef RES_STATION_H
#define RES_STATION_H

#include <stdint.h>
#include <stdbool.h>
#include "decode.h"

#define RES_STATION_SIZE 32

struct res_station
{
    bool busy;
    enum op op;
    uint32_t vj;
    uint32_t vk;
    uint32_t qj;
    uint32_t qk;
    uint32_t dest; // TODO: Remove this eventually
    uint32_t a;
    uint32_t inst_pc;
};

struct res_stations
{
    struct res_station stations[RES_STATION_SIZE];
    uint32_t num_stations;
};

struct res_stations *res_stations_init();        // Initialise reservation station
void res_stations_step(struct res_stations *rs); // Step reservation station
void res_stations_add(
    struct res_stations *rs,
    enum op op,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t dest,
    uint32_t inst_pc);                                           // Add instruction to reservation station
struct res_station res_stations_remove(struct res_stations *rs); // Remove instruction from reservation station
bool res_station_not_empty(struct res_stations *rs);             // Get number of stations in reservation station
// TODO: The res_stations_add args need to be fixed

void res_station_destroy(struct res_stations *rs); // Free reservation station

#endif // RES_STATION_H
