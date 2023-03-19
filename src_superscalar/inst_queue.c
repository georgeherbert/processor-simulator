#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "inst_queue.h"
#include "decode.h"

struct inst_queue *inst_queue_init()
{
    struct inst_queue *inst_queue = malloc(sizeof(struct inst_queue));
    if (inst_queue == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for instruction queue\n");
        exit(EXIT_FAILURE);
    }

    inst_queue->front = -1;
    inst_queue->rear = -1;

    return inst_queue;
}

bool inst_queue_is_not_empty(struct inst_queue *inst_queue)
{
    return inst_queue->front != -1;
}

bool inst_queue_is_not_full(struct inst_queue *inst_queue)
{
    return (inst_queue->rear + 1) % INST_QUEUE_SIZE != inst_queue->front;
}

void inst_queue_enqueue(struct inst_queue *inst_queue, struct decoded_inst inst)
{
    if (inst_queue_is_not_full(inst_queue))
    {
        if (inst_queue->front == -1)
        {
            inst_queue->front = 0;
        }
        inst_queue->rear = (inst_queue->rear + 1) % INST_QUEUE_SIZE;
        inst_queue->insts[inst_queue->rear] = inst;
    }
    else
    {
        fprintf(stderr, "Error: Cannot enqueue an instruction into a full instruction queue\n");
        exit(EXIT_FAILURE);
    }
}

struct decoded_inst inst_queue_dequeue(struct inst_queue *inst_queue)
{
    if (inst_queue_is_not_empty(inst_queue))
    {
        struct decoded_inst inst = inst_queue->insts[inst_queue->front];
        if (inst_queue->front == inst_queue->rear)
        {
            inst_queue->front = -1;
            inst_queue->rear = -1;
        }
        else
        {
            inst_queue->front = (inst_queue->front + 1) % INST_QUEUE_SIZE;
        }
        return inst;
    }
    else
    {
        fprintf(stderr, "Error: Cannot dequeue an empty instruction queue\n");
        exit(EXIT_FAILURE);
    }
}

enum op_type inst_queue_peek_op_type(struct inst_queue *inst_queue)
{
    if (inst_queue_is_not_empty(inst_queue))
    {
        return inst_queue->insts[0].op_type;
    }
    else
    {
        fprintf(stderr, "Error: Cannot peek an empty instruction queue\n");
        exit(EXIT_FAILURE);
    }
}

void inst_queue_destroy(struct inst_queue *inst_queue)
{
    free(inst_queue);
}
