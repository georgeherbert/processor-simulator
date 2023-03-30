#ifndef REORDER_BUFFER_H
#define REORDER_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "decoded_inst.h"
#include "reg.h"

#define REORDER_BUFFER_SIZE 32

struct reorder_buffer_entry
{
    uint32_t id;     // Reorder buffer entry IDs
    enum op_type op; // Operation type
    bool ready;      // Indicates whether the instruction has completed execution
    bool busy;       // Indicates whether the entry is busy
    uint32_t dest;   // rd_addr or store address
    uint32_t value;  // Value to be written to register or store
};

struct reorder_buffer
{
    struct reorder_buffer_entry entries_current[REORDER_BUFFER_SIZE]; // Current reorder buffer entries
    struct reorder_buffer_entry entries_next[REORDER_BUFFER_SIZE];    // Next reorder buffer entries
    int32_t front_current;                                            // Front of current reorder buffer
    int32_t rear_current;                                             // Rear of current reorder buffer
    int32_t front_next;                                               // Front of next reorder buffer
    int32_t rear_next;                                                // Rear of next reorder buffer
    struct reg *rob_full;                                             // Indicates whether reorder buffer is full
};

struct reorder_buffer *reorder_buffer_init(struct reg *rob_full); // Initialise reorder buffer
bool reorder_buffer_not_full(struct reorder_buffer *rob);         // Check if reorder buffer is not full
uint32_t reorder_buffer_add_entry(
    struct reorder_buffer *rob,
    enum op_type op,
    uint32_t dest);                                      // Add entry to reorder buffer
void reorder_buffer_destroy(struct reorder_buffer *rob); // Destroy reorder buffer

#endif // REORDER_BUFFER_H
