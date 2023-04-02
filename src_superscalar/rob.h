#ifndef REORDER_BUFFER_H
#define REORDER_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "decoded_inst.h"
#include "reg.h"
#include "cdb.h"

#define REORDER_BUFFER_SIZE 32

struct rob_entry
{
    uint32_t rob_id;      // Reorder buffer entry ID
    enum op_type op_type; // Operation type
    bool ready;           // Indicates whether the instruction has completed execution
    bool busy;            // Indicates whether the entry is busy
    uint32_t dest;        // rd_addr or store address
    uint32_t value;       // Value to be written to register or store
    uint32_t q;           // ROB ID of instruction that value is dependent on (stores only)
};

struct rob
{
    struct rob_entry queue_current[REORDER_BUFFER_SIZE]; // Current reorder buffer entries
    struct rob_entry queue_next[REORDER_BUFFER_SIZE];    // Next reorder buffer entries
    int32_t front_current;                               // Front of current reorder buffer
    int32_t front_next;                                  // Front of next reorder buffer
    int32_t rear_current;                                // Rear of current reorder buffer
    int32_t rear_next;                                   // Rear of next reorder buffer

    struct cdb *cdb; // Pointer to common data bus
};

struct rob *rob_init(struct cdb *cdb);                                                                  // Initialise reorder buffer
bool rob_full(struct rob *rob);                                                                         // Check if reorder buffer is full
bool rob_ready(struct rob *rob);                                                                        // Check if reorder buffer is ready
void rob_step(struct rob *rob);                                                                         // Step reorder buffer
uint32_t rob_enqueue(struct rob *rob, enum op_type op_type, uint32_t dest, uint32_t value, uint32_t q); // Enqueue instruction into reorder buffer
struct rob_entry rob_dequeue(struct rob *rob);                                                          // Dequeue instruction from reorder buffer
void rob_add_address(struct rob *rob, uint32_t rob_id, uint32_t addr);                                  // Add address to reorder buffer entry
bool rob_is_entry_ready(struct rob *rob, uint32_t id);                                                  // Check if reorder buffer entry is ready
uint32_t rob_get_entry_value(struct rob *rob, uint32_t id);                                             // Get value of reorder buffer entry
bool rob_earlier_stores(struct rob *rob, uint32_t rob_id, uint32_t addr);                               // Check if there are any earlier stores than the element at ROB ID with the same destination address
void rob_update_current(struct rob *rob);                                                               // Set current reorder buffer to next reorder buffer
void rob_destroy(struct rob *rob);                                                                      // Destroy reorder buffer

#endif // REORDER_BUFFER_H
