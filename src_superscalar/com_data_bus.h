#ifndef COM_DATA_BUS_H
#define COM_DATA_BUS_H

#include <stdint.h>
#include <stdbool.h>

struct com_data_bus_entry
{
    uint32_t rob_id; // Reorder buffer ID of result
    uint32_t val;    // val of result
};

struct com_data_bus
{
    struct com_data_bus_entry *entries; // Array of entries
    uint32_t num_entries;               // Number of entries (should be equal to the number of functional units)
};

struct com_data_bus *com_data_bus_init(uint32_t num_entries);                         // Initialise common data bus
void com_data_bus_add_entry(struct com_data_bus *cdb, uint32_t rob_id, uint32_t val); // Put val going to ROB ID on common data bus
bool com_data_bus_is_val_ready(struct com_data_bus *cdb, uint32_t rob_id);            // Check if val going to ROB ID is on common data bus
uint32_t com_data_bus_get_val(struct com_data_bus *cdb, uint32_t rob_id);             // Get val going to ROB ID from common data bus
void com_data_bus_step(struct com_data_bus *cdb);                                     // Step common data bus
void com_data_bus_destroy(struct com_data_bus *cdb);                                  // Free common data bus

#endif // COM_DATA_BUS_H
