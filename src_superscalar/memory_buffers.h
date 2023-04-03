#ifndef MEMORY_BUFFERS_H
#define MEMORY_BUFFERS_H

#include <stdint.h>
#include <stdbool.h>
#include "decode.h"
#include "reg_file.h"
#include "cdb.h"
#include "reg.h"
#include "rob.h"

struct memory_buffer
{
    uint32_t id;        // ID of memory buffer
    bool busy;          // Whether memory buffer is busy
    enum op op;         // Operation to perform
    uint32_t qj;        // Memory buffer ID of first operand
    uint32_t qk;        // Memory buffer ID of second operand
    uint32_t vj;        // val of first operand
    uint32_t vk;        // val of second operand
    uint32_t a;         // Immediate val or address
    uint32_t queue_pos; // Position in queue
    uint32_t rob_id;    // Destination ROB ID
};

struct memory_buffers
{
    struct memory_buffer *buffers_current; // Array of current memory buffers
    struct memory_buffer *buffers_next;    // Array of next memory buffers
    uint32_t num_buffers_in_queue_current; // Number of memory buffers in the current queue
    uint32_t num_buffers_in_queue_next;    // Number of memory buffers in the next queue
    uint32_t num_buffers;                  // Number of memory buffers

    struct reg_file *reg_file; // Pointer to register file
    struct cdb *cdb;           // Pointer to common data bus
    struct rob *rob;           // Pointer to reorder buffer
};

struct memory_buffers *memory_buffers_init(
    uint32_t num_buffers,
    uint32_t id_offset,
    struct reg_file *reg_file,
    struct cdb *cdb,
    struct rob *rob);                                // Initialise memory buffers
void memory_buffers_step(struct memory_buffers *rs); // Step memory buffers
void memory_buffers_enqueue(
    struct memory_buffers *mb,
    enum op op,
    uint32_t qj,
    uint32_t qk,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t rob_id);                                    // Add instruction to memory buffers
bool memory_buffers_all_busy(struct memory_buffers *mb); // Check if all memory buffers are busy
struct memory_buffer *memory_buffers_dequeue_memory(struct memory_buffers *mb, uint8_t id);
struct memory_buffer *memory_buffers_dequeue_address(struct memory_buffers *mb, uint8_t id);
void memory_buffers_add_address(struct memory_buffers *mb, uint32_t id, uint32_t address); // Add address to memory buffer
void memory_buffers_clear(struct memory_buffers *mb);                                      // Clear memory buffers on mispredict
void memory_buffers_update_current(struct memory_buffers *rs);                             // Update current memory buffers
void memory_buffers_destroy(struct memory_buffers *rs);                                    // Free memory buffers

#endif // MEMORY_BUFFERS_H
