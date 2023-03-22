#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "res_stations.h"
#include "reg_file.h"

struct res_stations *res_stations_init(
    uint32_t num_stations,
    uint32_t id_offset,
    struct reg_file *reg_file,
    struct com_data_bus *cdb)
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
                if (com_data_bus_is_val_ready(rs->cdb, rs->stations_next[i].qj))
                {
                    rs->stations_next[i].vj = com_data_bus_get_val(rs->cdb, rs->stations_next[i].qj);
                    rs->stations_next[i].qj = 0;
                }
            }
            if (rs->stations_next[i].qk != 0)
            {
                if (com_data_bus_is_val_ready(rs->cdb, rs->stations_next[i].qk))
                {
                    rs->stations_next[i].vk = com_data_bus_get_val(rs->cdb, rs->stations_next[i].qk);
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
    uint32_t dest,
    uint32_t inst_pc)
{
    if (res_stations_not_full(rs))
    {
        for (uint32_t i = 0; i < rs->num_stations; i++)
        {
            if (!rs->stations_next[i].busy)
            {
                rs->stations_next[i].busy = true;
                rs->stations_next[i].op = op;
                rs->stations_next[i].qj = qj;
                rs->stations_next[i].qk = qk;
                rs->stations_next[i].vj = vj;
                rs->stations_next[i].vk = vk;
                rs->stations_next[i].a = a;
                rs->stations_next[i].inst_pc = inst_pc;
                reg_file_set_reg_qi(rs->reg_file, dest, rs->stations_next[i].id);
                break;
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: Cannot add since all reservation stations are busy");
        exit(EXIT_FAILURE);
    }
}

struct res_station res_stations_remove(struct res_stations *rs)
{
    if (res_stations_is_ready(rs))
    {
        for (uint32_t i = 0; i < rs->num_stations; i++)
        {
            if (rs->stations_current[i].qj == 0 && rs->stations_current[i].qk == 0 && rs->stations_current[i].busy)
            {
                return rs->stations_current[i];
            }
        }
        fprintf(stderr, "Error: Cannot find a ready reservation station despite checking if there is one");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(stderr, "Error: Cannot remove since all busy reservation stations are awaiting operands");
        exit(EXIT_FAILURE);
    }
}

bool res_stations_is_ready(struct res_stations *rs)
{
    uint32_t is_ready = false;

    for (uint32_t i = 0; i < rs->num_stations; i++)
    {
        is_ready |= rs->stations_current[i].qj == 0 && rs->stations_current[i].qk == 0 && rs->stations_current[i].busy;
    }

    return is_ready;
}

bool res_stations_not_full(struct res_stations *rs)
{
    uint32_t is_full = true;

    for (uint32_t i = 0; i < rs->num_stations; i++)
    {
        is_full &= rs->stations_current[i].busy;
    }

    return !is_full;
}

void res_stations_set_station_not_busy(struct res_stations *rs, uint32_t id)
{
    for (uint32_t i = 0; i < rs->num_stations; i++)
    {
        if (rs->stations_next[i].id == id)
        {
            rs->stations_next[i].busy = false;
            break;
        }
    }
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
