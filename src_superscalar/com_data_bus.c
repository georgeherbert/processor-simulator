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

    cbd->entries = malloc(sizeof(struct com_data_bus_entry) * num_entries);
    if (cbd->entries == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for common data bus entries");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < num_entries; i++)
    {
        cbd->entries[i].qi = 0;
        cbd->entries[i].value = 0;
    }

    cbd->num_entries = num_entries;

    return cbd;
}

void com_data_bus_add_entry(struct com_data_bus *cbd, uint32_t qi, uint32_t value)
{
    for (uint32_t i = 0; i < cbd->num_entries; i++)
    {
        if (cbd->entries[i].qi == 0)
        {
            cbd->entries[i].qi = qi;
            cbd->entries[i].value = value;
            return;
        }
    }
}

bool com_data_bus_is_value_ready(struct com_data_bus *cbd, uint32_t qi)
{
    for (uint32_t i = 0; i < cbd->num_entries; i++)
    {
        if (cbd->entries[i].qi == qi)
        {
            return true;
        }
    }

    return false;
}

uint32_t com_data_bus_get_value(struct com_data_bus *cbd, uint32_t qi)
{
    for (uint32_t i = 0; i < cbd->num_entries; i++)
    {
        if (cbd->entries[i].qi == qi)
        {
            return cbd->entries[i].value;
        }
    }

    fprintf(stderr, "Error: Could not find value for qi %d", qi);
    exit(EXIT_FAILURE);
}

void com_data_bus_step(struct com_data_bus *cbd)
{
    for (uint32_t i = 0; i < cbd->num_entries; i++)
    {
        // printf("qi: %d, value: %d\n", cbd->entries[i].qi, cbd->entries[i].value);
        cbd->entries[i].qi = 0;
    }
}

void com_data_bus_destroy(struct com_data_bus *cbd)
{
    free(cbd->entries);
    free(cbd);
}
