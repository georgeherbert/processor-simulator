#ifndef INST_QUEUE_H
#define INST_QUEUE_H

#include "decoded_inst.h"

#define INST_QUEUE_SIZE 16

struct inst_queue
{
    struct decoded_inst insts[INST_QUEUE_SIZE];
};

struct inst_queue *inst_queue_init();                                          // Initialise instruction queue
void inst_queue_enqueue(struct inst_queue *inst_queue, struct decoded_inst inst); // Push instruction to queue
struct decoded_inst inst_queue_dequeue(struct inst_queue *inst_queue);             // Pop instruction from queue
enum op_type inst_queue_peek_op_type(struct inst_queue *inst_queue);           // Peek at next instruction's op type
void inst_queue_destroy(struct inst_queue *inst_queue);                        // Free instruction queue

#endif // INST_QUEUE_H
