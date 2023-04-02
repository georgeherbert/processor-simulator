#ifndef INST_QUEUE_H
#define INST_QUEUE_H

#include <stdbool.h>
#include "decoded_inst.h"
#include "reg.h"

#define INST_QUEUE_SIZE 32

struct inst_queue
{
    struct decoded_inst queue_current[INST_QUEUE_SIZE];
    struct decoded_inst queue_next[INST_QUEUE_SIZE];
    int32_t front_current;
    int32_t front_next;
    int32_t rear_next;
    int32_t rear_current;
};

struct inst_queue *inst_queue_init();                                             // Initialise instruction queue
void inst_queue_enqueue(struct inst_queue *inst_queue, struct decoded_inst inst); // Push instruction to queue
bool inst_queue_full(struct inst_queue *iq);                                      // Check if instruction queue is full
bool inst_queue_empty(struct inst_queue *iq);                                     // Check if instruction queue is empty
struct decoded_inst inst_queue_dequeue(struct inst_queue *inst_queue);            // Pop instruction from queue
enum op_type inst_queue_peek_op_type(struct inst_queue *inst_queue);              // Peek at next instruction's op type
void inst_queue_update_current(struct inst_queue *inst_queue);                    // Set current instruction queue to next instruction queue
void inst_queue_destroy(struct inst_queue *inst_queue);                           // Free instruction queue

#endif // INST_QUEUE_H
