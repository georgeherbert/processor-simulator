#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>
#include "inst_queue.h"
#include "decoded_inst.h"
#include "reg.h"

struct decode_unit
{
    struct reg *reg_inst;          // Pointer to instruction register
    struct reg *reg_inst_pc;       // Pointer to program counter of instruction in instruction register
    struct inst_queue *inst_queue; // Pointer to instruction queue
};

struct decode_unit *decode_init(
    struct reg *reg_inst,
    struct reg *reg_inst_pc,
    struct inst_queue *inst_queue);                   // Initialise decode unit
void decode_step(struct decode_unit *decode_unit);    // Step decode unit
void decode_destroy(struct decode_unit *decode_unit); // Free decode unit

#endif // DECODE_H
