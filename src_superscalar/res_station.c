#include <stdlib.h>
#include <stdio.h>
#include "res_station.h"

struct res_station *res_station_init()
{
    struct res_station *rs = malloc(sizeof(struct res_station));
    if (rs == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for reservation station");
        exit(EXIT_FAILURE);
    }

    rs->num_entries = 0;

    return rs;
}

void res_station_step(struct res_station *rs)
{
}

void res_station_add(
    struct res_station *rs,
    enum op op,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t dest,
    uint32_t inst_pc)
{
    // TODO: This shouldn't be zero and the attributes are hacky for now
    rs->entries[0].busy = true;
    rs->entries[0].op = op;
    rs->entries[0].vj = vj;
    rs->entries[0].vk = vk;
    rs->entries[0].qj = 0;
    rs->entries[0].qk = 0;
    rs->entries[0].dest = dest;

    rs->entries[0].a = a;
    rs->entries[0].inst_pc = inst_pc;

    rs->num_entries++;
}

struct res_station_entry res_station_remove(struct res_station *rs)
{
    rs->num_entries--;
    return rs->entries[0]; // TODO: This shouldn't be zero
}

uint32_t res_station_get_num_entries(struct res_station *rs)
{
    return rs->num_entries;
}

void res_station_destroy(struct res_station *rs)
{
    free(rs);
}
