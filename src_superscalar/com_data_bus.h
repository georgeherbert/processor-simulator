#ifndef COM_DATA_BUS_H
#define COM_DATA_BUS_H

#include <stdint.h>
#include <stdbool.h>

struct com_data_bus_entry
{
    uint32_t qi;    // ID of reservation station that produced the result
    uint32_t value; // Value of result
};

struct com_data_bus
{
    struct com_data_bus_entry *entries; // Array of entries
    uint32_t num_entries;               // Number of entries (should be equal to the number of functional units)
};

struct com_data_bus *com_data_bus_init(uint32_t num_entries);                       // Initialise common data bus
void com_data_bus_add_entry(struct com_data_bus *cbd, uint32_t qi, uint32_t value); // Put value from qi on common data bus
bool com_data_bus_is_value_ready(struct com_data_bus *cbd, uint32_t qi);            // Check if value from qi is on common data bus
uint32_t com_data_bus_get_value(struct com_data_bus *cbd, uint32_t qi);             // Get value from qi from common data bus
void com_data_bus_step(struct com_data_bus *cbd);                                   // Step common data bus
void com_data_bus_destroy(struct com_data_bus *cbd);                                // Free common data bus

#endif // COM_DATA_BUS_H
