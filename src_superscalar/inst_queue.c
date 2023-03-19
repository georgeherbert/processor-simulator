#include <stdlib.h>
#include <stdio.h>
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

    return inst_queue;
}

// TODO: Implement this properly
void inst_queue_enqueue(struct inst_queue *inst_queue, struct decoded_inst inst)
{
    inst_queue->insts[0] = inst;
}

// TODO: Implement this properly
struct decoded_inst inst_queue_dequeue(struct inst_queue *inst_queue)
{
    return inst_queue->insts[0];
}

enum op_type inst_queue_peek_op_type(struct inst_queue *inst_queue)
{
    return inst_queue->insts[0].op_type;
}

void inst_queue_destroy(struct inst_queue *inst_queue)
{
    free(inst_queue);
}
