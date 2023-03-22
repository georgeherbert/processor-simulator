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

    inst_queue->current.front = -1;
    inst_queue->current.rear = -1;
    inst_queue->next.front = -1;
    inst_queue->next.rear = -1;

    return inst_queue;
}

bool inst_queue_is_empty(struct inst_queue_inst iqi)
{
    return iqi.front == -1;
}

bool inst_queue_is_not_empty(struct inst_queue *inst_queue)
{
    return !(inst_queue_is_empty(inst_queue->next));
}

bool inst_queue_is_full(struct inst_queue_inst iqi)
{
    return (iqi.front == iqi.rear + 1) || (iqi.front == 0 && iqi.rear == INST_QUEUE_SIZE - 1);
}

bool inst_queue_is_not_full(struct inst_queue *inst_queue)
{
    return (!(inst_queue_is_full(inst_queue->next)));
}

void inst_queue_enqueue(struct inst_queue *inst_queue, struct decoded_inst inst)
{
    // switch (inst.op_type)
    // {
    // case AL:
    //     printf("Enqueue: AL\n");
    //     break;
    // case BRANCH:
    //     printf("Enqueue: BRANCH\n");
    //     break;
    // case MEMORY:
    //     printf("Enqueue: MEMORY\n");
    //     break;
    // }

    if (inst_queue_is_full(inst_queue->next))
    {
        fprintf(stderr, "Error: Cannot enqueue an instruction into a full instruction queue\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        if (inst_queue->next.front == -1)
        {
            inst_queue->next.front = 0;
        }
        inst_queue->next.rear = (inst_queue->next.rear + 1) % INST_QUEUE_SIZE;
        inst_queue->next.insts[inst_queue->next.rear] = inst;
    }
}

struct decoded_inst inst_queue_dequeue(struct inst_queue *inst_queue)
{
    if (inst_queue_is_empty(inst_queue->next))
    {
        fprintf(stderr, "Error: Cannot dequeue an instruction from an empty instruction queue\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        struct decoded_inst inst = inst_queue->next.insts[inst_queue->next.front];
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
}

enum op_type inst_queue_peek_op_type(struct inst_queue *inst_queue)
{
    if (inst_queue_is_not_empty(inst_queue))
    {
        return inst_queue->next.insts[inst_queue->next.front].op_type;
    }
    else
    {
        fprintf(stderr, "Error: Cannot peek an empty instruction queue\n");
        exit(EXIT_FAILURE);
    }
}

void inst_queue_update_current(struct inst_queue *inst_queue)
{
    inst_queue->current = inst_queue->next;
    // printf("Front: %d, Rear: %d\n", inst_queue->current.front, inst_queue->current.rear);
}

void inst_queue_destroy(struct inst_queue *inst_queue)
{
    free(inst_queue);
}
