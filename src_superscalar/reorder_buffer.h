#ifndef REORDER_BUFFER_H
#define REORDER_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "decoded_inst.h"

#define REORDER_BUFFER_SIZE 32

struct reorder_buffer_entry
{
    uint32_t id;     // Reorder buffer entry ID
    enum op_type op; // Operation type
    bool ready;      // Indicates whether the instruction has completed execution
    uint32_t dest;   // rd_addr or store address
    uint32_t value;  // Value to be written to register or store
};

struct reorder_buffer
{
    struct reorder_buffer_entry entries[REORDER_BUFFER_SIZE]; // Reorder buffer entries
    int32_t front;                                            // Front of circular queue
    int32_t rear;                                             // Rear of circular queue
};

bool reorder_buffer_not_full(struct reorder_buffer *rob); // Check if reorder buffer is not full
uint32_t reorder_buffer_add_entry(
    struct reorder_buffer *rob,
    enum op_type op,
    uint32_t dest); // Add entry to reorder buffer

#endif // REORDER_BUFFER_H
