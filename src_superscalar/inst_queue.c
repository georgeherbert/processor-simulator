#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <strings.h>
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

    inst_queue->front_current = -1;
    inst_queue->front_next = -1;
    inst_queue->rear_next = -1;

    inst_queue->inst_queue_empty = inst_queue_empty;
    inst_queue->inst_queue_full = inst_queue_full;

    return inst_queue;
}

void inst_queue_enqueue(struct inst_queue *inst_queue, struct decoded_inst inst)
{
    if (inst_queue->front_next == -1)
    {
        inst_queue->front_next = 0;
    }
    inst_queue->rear_next = (inst_queue->rear_next + 1) % INST_QUEUE_SIZE;
    inst_queue->queue_next[inst_queue->rear_next] = inst;
}

struct decoded_inst inst_queue_dequeue(struct inst_queue *inst_queue)
{
    struct decoded_inst inst = inst_queue->queue_current[inst_queue->front_current];
    if (inst_queue->front_next == inst_queue->rear_next)
    {
        inst_queue->front_next = -1;
        inst_queue->rear_next = -1;
    }
    else
    {
        inst_queue->front_next = (inst_queue->front_next + 1) % INST_QUEUE_SIZE;
    }
    return inst;
}

enum op_type inst_queue_peek_op_type(struct inst_queue *inst_queue)
{
    return inst_queue->queue_current[inst_queue->front_current].op_type;
}

void inst_queue_step(struct inst_queue *inst_queue)
{
    bool is_empty = inst_queue->front_next == -1;
    bool is_full = (inst_queue->rear_next + 1) % INST_QUEUE_SIZE == inst_queue->front_next;
    reg_write(inst_queue->inst_queue_empty, is_empty);
    reg_write(inst_queue->inst_queue_full, is_full);
}

void inst_queue_update_current(struct inst_queue *inst_queue)
{
    memcpy(inst_queue->queue_current, inst_queue->queue_next, sizeof(struct decoded_inst) * INST_QUEUE_SIZE);
    inst_queue->front_current = inst_queue->front_next;
}

void inst_queue_destroy(struct inst_queue *inst_queue)
{
    free(inst_queue);
}
