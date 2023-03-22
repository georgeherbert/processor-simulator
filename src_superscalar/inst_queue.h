#ifndef INST_QUEUE_H
#define INST_QUEUE_H

#include <stdbool.h>
#include "decoded_inst.h"
#include "reg.h"

#define INST_QUEUE_SIZE 32

struct inst_queue_inst
{
    struct decoded_inst insts[INST_QUEUE_SIZE];
    int32_t front;
    int32_t rear;
};

struct inst_queue
{
    struct inst_queue_inst current;
    struct inst_queue_inst next;
    struct reg *inst_queue_empty;
    struct reg *inst_queue_full;
};

struct inst_queue *inst_queue_init(struct reg *inst_queue_empty, struct reg *inst_queue_full); // Initialise instruction queue
void inst_queue_enqueue(struct inst_queue *inst_queue, struct decoded_inst inst);              // Push instruction to queue
struct decoded_inst inst_queue_dequeue(struct inst_queue *inst_queue);                         // Pop instruction from queue
enum op_type inst_queue_peek_op_type(struct inst_queue *inst_queue);                           // Peek at next instruction's op type
void inst_queue_step(struct inst_queue *inst_queue);                                           // Update instruction queue registers
void inst_queue_update_current(struct inst_queue *inst_queue);                                 // Set current instruction queue to next instruction queue
void inst_queue_destroy(struct inst_queue *inst_queue);                                        // Free instruction queue

#endif // INST_QUEUE_H
