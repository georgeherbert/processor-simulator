#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>
#include "inst_queue.h"
#include "decoded_inst.h"

struct decode_unit
{
    uint32_t *reg_inst;            // Pointer to instruction register
    uint32_t *reg_pc;              // Pointer to program counter
    struct inst_queue *inst_queue; // Pointer to instruction queue
};

struct decode_unit *decode_init(
    uint32_t *reg_inst,
    uint32_t *reg_pc,
    struct inst_queue *inst_queue);                   // Initialise decode unit
void decode_step(struct decode_unit *decode_unit);    // Step decode unit
void decode_destroy(struct decode_unit *decode_unit); // Free decode unit

#endif // DECODE_H
