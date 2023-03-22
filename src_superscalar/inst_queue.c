#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "inst_queue.h"
#include "decode.h"

struct inst_queue *inst_queue_init(struct reg *inst_queue_empty, struct reg *inst_queue_full)
{
    struct inst_queue *inst_queue = malloc(sizeof(struct inst_queue));
    if (inst_queue == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for instruction queue\n");
        exit(EXIT_FAILURE);
    }

    inst_queue->inst_queue_empty = inst_queue_empty;
    inst_queue->inst_queue_full = inst_queue_full;

    inst_queue->current.front = -1;
    inst_queue->current.rear = -1;
    inst_queue->next.front = -1;
    inst_queue->next.rear = -1;

    return inst_queue;
}

bool inst_queue_is_full(struct inst_queue *inst_queue)
{
    return (inst_queue->next.rear + 1) % INST_QUEUE_SIZE == inst_queue->next.front;
}

bool inst_queue_is_empty(struct inst_queue *inst_queue)
{
    return inst_queue->next.front == -1;
}

void inst_queue_enqueue(struct inst_queue *inst_queue, struct decoded_inst inst)
{
    if (inst_queue->next.front == -1)
    {
        inst_queue->next.front = 0;
    }
    inst_queue->next.rear = (inst_queue->next.rear + 1) % INST_QUEUE_SIZE;
    inst_queue->next.insts[inst_queue->next.rear] = inst;
}

struct decoded_inst inst_queue_dequeue(struct inst_queue *inst_queue)
{
    struct decoded_inst inst = inst_queue->current.insts[inst_queue->current.front];
    if (inst_queue->next.front == inst_queue->next.rear)
    {
        inst_queue->next.front = -1;
        inst_queue->next.rear = -1;
    }
    else
    {
        inst_queue->next.front = (inst_queue->next.front + 1) % INST_QUEUE_SIZE;
    }

    return inst;
}

enum op_type inst_queue_peek_op_type(struct inst_queue *inst_queue)
{
    return inst_queue->current.insts[inst_queue->current.front].op_type;
}

void inst_queue_step(struct inst_queue *inst_queue)
{
    reg_write(inst_queue->inst_queue_empty, inst_queue_is_empty(inst_queue));
    reg_write(inst_queue->inst_queue_full, inst_queue_is_full(inst_queue));
}

void inst_queue_update_current(struct inst_queue *inst_queue)
{
    inst_queue->current = inst_queue->next;
}

void inst_queue_destroy(struct inst_queue *inst_queue)
{
    free(inst_queue);
}
