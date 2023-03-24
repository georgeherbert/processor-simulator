#ifndef MEMORY_BUFFERS_H
#define MEMORY_BUFFERS_H

#include <stdint.h>
#include <stdbool.h>
#include "decode.h"
#include "reg_file.h"
#include "com_data_bus.h"
#include "reg.h"

struct memory_buffer
{
    uint32_t id; // ID of memory buffer
    bool busy;   // Whether memory buffer is busy
    enum op op;  // Operation to perform
    uint32_t qj; // Memory buffer ID of first operand
    uint32_t qk; // Memory buffer ID of second operand
    uint32_t vj; // val of first operand
    uint32_t vk; // val of second operand
    uint32_t a;  // Immediate val or address
};

struct memory_buffers
{
    struct memory_buffer *buffers_current; // Array of current memory buffers
    struct memory_buffer *buffers_next;    // Array of next memory buffers
    struct reg_file *reg_file;             // Pointer to register file
    uint32_t num_buffers;                  // Number of memory buffers
    struct com_data_bus *cdb;              // Pointer to common data bus
    struct reg *memory_buffers_all_busy;   // Pointer to register that indicates if all memory buffers are busy
    struct reg *memory_buffers_ready;      // Pointer to register that indicates if any memory buffers are ready
};

struct memory_buffers *memory_buffers_init(
    uint32_t num_buffers,
    uint32_t id_offset,
    struct reg_file *reg_file,
    struct com_data_bus *cdb,
    struct reg *memory_buffers_all_busy,
    struct reg *memory_buffers_ready);               // Initialise memory buffers
void memory_buffers_step(struct memory_buffers *rs); // Step memory buffers
void memory_buffers_add(
    struct memory_buffers *rs,
    enum op op,
    uint32_t qj,
    uint32_t qk,
    uint32_t vj,
    uint32_t vk,
    uint32_t a,
    uint32_t dest);                                                              // Add instruction to memory buffers
struct memory_buffer memory_buffers_remove(struct memory_buffers *rs);           // Remove instruction from memory buffers
void memory_buffers_set_buffer_not_busy(struct memory_buffers *rs, uint32_t id); // Set a memory buffer to not busy
void memory_buffers_update_current(struct memory_buffers *rs);                   // Update current memory buffers
void memory_buffers_destroy(struct memory_buffers *rs);                          // Free memory buffers

#endif // MEMORY_BUFFERS_H
