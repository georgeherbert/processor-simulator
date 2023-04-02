#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "res_stations.h"
#include "reg_file.h"
#include "reg.h"

struct res_stations *res_stations_init(
    uint32_t num_stations,
    uint32_t id_offset,
    struct reg_file *reg_file,
    struct cdb *cdb)
{
    struct res_stations *rs = malloc(sizeof(struct res_stations));
    if (rs == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for reservation stations");
        exit(EXIT_FAILURE);
    }

    rs->stations_current = malloc(sizeof(struct res_station) * num_stations);
    if (rs->stations_current == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for current individual reservation stations");
        exit(EXIT_FAILURE);
    }

    rs->stations_next = malloc(sizeof(struct res_station) * num_stations);
    if (rs->stations_next == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for next individual reservation stations");
        exit(EXIT_FAILURE);
    }

    rs->num_stations = num_stations;
    rs->reg_file = reg_file;
    rs->cdb = cdb;

    for (uint32_t i = 0; i < num_stations; i++)
    {
        rs->stations_current[i].id = i + id_offset;
        rs->stations_current[i].busy = false;
        rs->stations_next[i].id = i + id_offset;
        rs->stations_next[i].busy = false;
    }

    return rs;
}

void res_stations_step(struct res_stations *rs)
{
    for (uint32_t i = 0; i < rs->num_stations; i++)
    {
        if (rs->stations_next[i].busy)
        {
            if (rs->stations_next[i].qj != 0)
            {
                if (cdb_is_val_ready(rs->cdb, rs->stations_next[i].qj))
                {
                    rs->stations_next[i].vj = cdb_get_val(rs->cdb, rs->stations_next[i].qj);
                    rs->stations_next[i].qj = 0;
                }
            }
            if (rs->stations_next[i].qk != 0)
            {
                if (cdb_is_val_ready(rs->cdb, rs->stations_next[i].qk))
                {
                    rs->stations_next[i].vk = cdb_get_val(rs->cdb, rs->stations_next[i].qk);
                    rs->stations_next[i].qk = 0;
                }
            }
        }
    }
}

void res_stations_add(
    struct res_stations *rs,
    enum op op,
    uint32_t qj,
    uint32_t qk,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t rob_id,
    uint32_t inst_pc)
{
    for (uint32_t i = 0; i < rs->num_stations; i++)
    {
        if (!rs->stations_current[i].busy)
        {
            rs->stations_next[i].busy = true;
            rs->stations_next[i].op = op;
            rs->stations_next[i].qj = qj;
            rs->stations_next[i].qk = qk;
            rs->stations_next[i].vj = vj;
            rs->stations_next[i].vk = vk;
            rs->stations_next[i].a = a;
            rs->stations_next[i].rob_id = rob_id;
            rs->stations_next[i].inst_pc = inst_pc;
            break;
        }
    }
}

bool res_stations_all_busy(struct res_stations *rs)
{
    bool all_busy = true;
    for (uint32_t i = 0; i < rs->num_stations; i++)
    {
        all_busy &= rs->stations_current[i].busy;
    }
    return all_busy;
}

struct res_station *res_stations_remove(struct res_stations *rs)
{
    for (uint32_t i = 0; i < rs->num_stations; i++)
    {
        if (rs->stations_current[i].qj == 0 && rs->stations_current[i].qk == 0 && rs->stations_current[i].busy)
        {
            rs->stations_next[i].busy = false;
            return &rs->stations_current[i];
        }
    }
    return NULL;
}

void res_stations_update_current(struct res_stations *rs)
{
    memcpy(rs->stations_current, rs->stations_next, sizeof(struct res_station) * rs->num_stations);
}

void res_stations_destroy(struct res_stations *rs)
{
    free(rs->stations_current);
    free(rs->stations_next);
    free(rs);
}
