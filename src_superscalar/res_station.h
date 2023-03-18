#ifndef RES_STATION_H
#define RES_STATION_H

#include <stdint.h>
#include <stdbool.h>
#include "decode.h"

#define RES_STATION_SIZE 32

struct res_station_entry
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

struct res_station
{
    struct res_station_entry entries[RES_STATION_SIZE];
    uint32_t num_entries;
};

struct res_station *res_station_init();        // Initialise reservation station
void res_station_step(struct res_station *rs); // Step reservation station
void res_station_add(
    struct res_station *rs,
    enum op op,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t dest,
    uint32_t inst_pc);                                               // Add instruction to reservation station
struct res_station_entry res_station_remove(struct res_station *rs); // Remove instruction from reservation station
uint32_t res_station_get_num_entries(struct res_station *rs);        // Get number of entries in reservation station
// TODO: The res_station_add args need to be fixed

void res_station_destroy(struct res_station *rs); // Free reservation station

#endif // RES_STATION_H
