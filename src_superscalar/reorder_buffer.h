#ifndef REORDER_BUFFER_H
#define REORDER_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "decoded_inst.h"
#include "reg.h"
#include "com_data_bus.h"

#define REORDER_BUFFER_SIZE 32

struct reorder_buffer_entry
{
    uint32_t rob_id;      // Reorder buffer entry ID
    enum op_type op_type; // Operation type
    bool ready;           // Indicates whether the instruction has completed execution
    bool busy;            // Indicates whether the entry is busy
    uint32_t dest;        // rd_addr or store address
    uint32_t value;       // Value to be written to register or store
    uint32_t q;           // ROB ID of instruction that value is dependent on (stores only)
};

struct reorder_buffer
{
    struct reorder_buffer_entry queue_current[REORDER_BUFFER_SIZE]; // Current reorder buffer entries
    struct reorder_buffer_entry queue_next[REORDER_BUFFER_SIZE];    // Next reorder buffer entries
    int32_t front_current;                                          // Front of current reorder buffer
    int32_t front_next;                                             // Front of next reorder buffer
    int32_t rear_next;                                              // Rear of next reorder buffer
    struct reg *rob_full;                                           // Indicates whether reorder buffer is full
    struct com_data_bus *cdb;                                       // Pointer to common data bus
    struct reg *rob_ready;                                          // Indicates whether reorder buffer is ready
};

struct reorder_buffer *reorder_buffer_init(
    struct reg *rob_full,
    struct com_data_bus *cdb,
    struct reg *rob_ready);                                                                                                   // Initialise reorder buffer
void reorder_buffer_step(struct reorder_buffer *rob);                                                                         // Step reorder buffer
uint32_t reorder_buffer_enqueue(struct reorder_buffer *rob, enum op_type op_type, uint32_t dest, uint32_t value, uint32_t q); // Enqueue instruction into reorder buffer
struct reorder_buffer_entry reorder_buffer_dequeue(struct reorder_buffer *rob);                                               // Dequeue instruction from reorder buffer
void reorder_buffer_add_address(struct reorder_buffer *rob, uint32_t rob_id, uint32_t addr);                                  // Add address to reorder buffer entry
bool reorder_buffer_is_entry_ready(struct reorder_buffer *rob, uint32_t id);                                                  // Check if reorder buffer entry is ready
uint32_t reorder_buffer_get_entry_value(struct reorder_buffer *rob, uint32_t id);                                             // Get value of reorder buffer entry
bool reorder_buffer_earlier_stores(struct reorder_buffer *rob, uint32_t rob_id, uint32_t addr);                               // Check if there are any earlier stores than the element at ROB ID with the same destination address
void reorder_buffer_update_current(struct reorder_buffer *rob);                                                               // Set current reorder buffer to next reorder buffer
void reorder_buffer_destroy(struct reorder_buffer *rob);                                                                      // Destroy reorder buffer

#endif // REORDER_BUFFER_H
