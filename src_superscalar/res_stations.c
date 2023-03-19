#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "res_stations.h"

struct res_stations *res_stations_init(uint32_t num_stations, uint32_t id_offset)
{
    struct res_stations *rs = malloc(sizeof(struct res_stations));
    if (rs == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for reservation stations");
        exit(EXIT_FAILURE);
    }

    rs->stations = malloc(sizeof(struct res_station) * num_stations);
    if (rs->stations == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for individual reservation stations");
        exit(EXIT_FAILURE);
    }

    rs->num_stations = num_stations;

    for (uint32_t i = 0; i < num_stations; i++)
    {
        rs->stations[i].id = i + id_offset;
        rs->stations[i].busy = false;
    }

    return rs;
}

void res_stations_step(struct res_stations *rs)
{
}

void res_stations_add(
    struct res_stations *rs,
    enum op op,
    uint32_t qj,
    uint32_t qk,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t dest,
    uint32_t inst_pc)
{
    // TODO: This shouldn't be zero and the attributes are hacky for now
    rs->stations[0].busy = true;
    rs->stations[0].op = op;
    rs->stations[0].qj = qj;
    rs->stations[0].qk = qk;
    rs->stations[0].vj = vj;
    rs->stations[0].vk = vk;
    rs->stations[0].dest = dest;

    rs->stations[0].a = a;
    rs->stations[0].inst_pc = inst_pc;
}

struct res_station res_stations_remove(struct res_stations *rs)
{
    rs->stations[0].busy = false;
    return rs->stations[0]; // TODO: This shouldn't be zero
}

bool res_stations_not_empty(struct res_stations *rs)
{
    uint32_t is_empty = true;

    for (uint32_t i = 0; i < rs->num_stations; i++)
    {
        is_empty &= !rs->stations[i].busy;
    }

    return !is_empty;
}

void res_stations_destroy(struct res_stations *rs)
{
    free(rs->stations);
    free(rs);
}
