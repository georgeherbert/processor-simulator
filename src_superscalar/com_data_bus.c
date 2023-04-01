#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "com_data_bus.h"

struct com_data_bus *com_data_bus_init(uint32_t num_entries)
{
    struct com_data_bus *cdb = malloc(sizeof(struct com_data_bus));
    if (cdb == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for common data bus");
        exit(EXIT_FAILURE);
    }

    cdb->entries = malloc(sizeof(struct com_data_bus_entry) * num_entries);
    if (cdb->entries == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for common data bus entries");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < num_entries; i++)
    {
        cdb->entries[i].rob_id = 0;
        cdb->entries[i].val = 0;
    }

    cdb->num_entries = num_entries;

    return cdb;
}

void com_data_bus_add_entry(struct com_data_bus *cdb, uint32_t rob_id, uint32_t val)
{
    for (uint32_t i = 0; i < cdb->num_entries; i++)
    {
        if (cdb->entries[i].rob_id == 0)
        {
            // printf("CDB #%d: %d\n", rob_id, val);
            cdb->entries[i].rob_id = rob_id;
            cdb->entries[i].val = val;
            return;
        }
    }
}

bool com_data_bus_is_val_ready(struct com_data_bus *cdb, uint32_t rob_id)
{
    for (uint32_t i = 0; i < cdb->num_entries; i++)
    {
        if (cdb->entries[i].rob_id == rob_id)
        {
            return true;
        }
    }

    return false;
}

uint32_t com_data_bus_get_val(struct com_data_bus *cdb, uint32_t rob_id)
{
    for (uint32_t i = 0; i < cdb->num_entries; i++)
    {
        if (cdb->entries[i].rob_id == rob_id)
        {
            return cdb->entries[i].val;
        }
    }
    fprintf(stderr, "Error: Could not find value for rob_id %d in common data bus", rob_id);
    exit(EXIT_FAILURE);
}

void com_data_bus_step(struct com_data_bus *cdb)
{
    for (uint32_t i = 0; i < cdb->num_entries; i++)
    {
        cdb->entries[i].rob_id = 0;
    }
}

void com_data_bus_destroy(struct com_data_bus *cdb)
{
    free(cdb->entries);
    free(cdb);
}
