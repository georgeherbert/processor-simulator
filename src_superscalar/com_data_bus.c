#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "com_data_bus.h"

struct com_data_bus *com_data_bus_init(uint32_t num_entries)
{
    struct com_data_bus *cbd = malloc(sizeof(struct com_data_bus));
    if (cbd == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for common data bus");
        exit(EXIT_FAILURE);
    }

    cbd->current.entries = malloc(sizeof(struct com_data_bus_entry) * num_entries);
    if (cbd->current.entries == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for current common data bus entries");
        exit(EXIT_FAILURE);
    }

    cbd->next.entries = malloc(sizeof(struct com_data_bus_entry) * num_entries);
    if (cbd->next.entries == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for next data bus entries");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < num_entries; i++)
    {
        cbd->current.entries[i].qi = 0;
        cbd->current.entries[i].val = 0;
        cbd->next.entries[i].qi = 0;
        cbd->next.entries[i].val = 0;
    }

    cbd->num_entries = num_entries;

    return cbd;
}

void com_data_bus_add_entry(struct com_data_bus *cbd, uint32_t qi, uint32_t val)
{
    for (uint32_t i = 0; i < cbd->num_entries; i++)
    {
        if (cbd->next.entries[i].qi == 0)
        {
            cbd->next.entries[i].qi = qi;
            cbd->next.entries[i].val = val;
            return;
        }
    }
}

bool com_data_bus_is_val_ready(struct com_data_bus *cbd, uint32_t qi)
{
    for (uint32_t i = 0; i < cbd->num_entries; i++)
    {
        if (cbd->current.entries[i].qi == qi)
        {
            return true;
        }
    }

    return false;
}

uint32_t com_data_bus_get_val(struct com_data_bus *cbd, uint32_t qi)
{
    for (uint32_t i = 0; i < cbd->num_entries; i++)
    {
        if (cbd->current.entries[i].qi == qi)
        {
            return cbd->current.entries[i].val;
        }
    }

    fprintf(stderr, "Error: Could not find val for qi %d", qi);
    exit(EXIT_FAILURE);
}

void com_data_bus_step(struct com_data_bus *cbd)
{
    for (uint32_t i = 0; i < cbd->num_entries; i++)
    {
        // printf("qi: %d, val: %d\n", cbd->entries[i].qi, cbd->entries[i].val);
        cbd->next.entries[i].qi = 0;
    }
}

void com_data_bus_update_current(struct com_data_bus *cbd)
{
    for (uint32_t i = 0; i < cbd->num_entries; i++)
    {
        cbd->current.entries[i].qi = cbd->next.entries[i].qi;
        cbd->current.entries[i].val = cbd->next.entries[i].val;
    }
}

void com_data_bus_destroy(struct com_data_bus *cbd)
{
    free(cbd->current.entries);
    free(cbd->next.entries);
    free(cbd);
}
