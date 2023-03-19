#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "res_station.h"

struct res_stations *res_stations_init()
{
    struct res_stations *rs = malloc(sizeof(struct res_stations));
    if (rs == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for reservation station");
        exit(EXIT_FAILURE);
    }

    rs->num_stations = 0;

    return rs;
}

void res_stations_step(struct res_stations *rs)
{
}

void res_stations_add(
    struct res_stations *rs,
    enum op op,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t dest,
    uint32_t inst_pc)
{
    // TODO: This shouldn't be zero and the attributes are hacky for now
    rs->stations[0].busy = true;
    rs->stations[0].op = op;
    rs->stations[0].vj = vj;
    rs->stations[0].vk = vk;
    rs->stations[0].qj = 0;
    rs->stations[0].qk = 0;
    rs->stations[0].dest = dest;

    rs->stations[0].a = a;
    rs->stations[0].inst_pc = inst_pc;

    rs->num_stations++;
}

struct res_station res_stations_remove(struct res_stations *rs)
{
    rs->num_stations--;
    return rs->stations[0]; // TODO: This shouldn't be zero
}

bool res_station_not_empty(struct res_stations *rs)
{
    return rs->num_stations != 0;
}

void res_station_destroy(struct res_stations *rs)
{
    free(rs);
}
