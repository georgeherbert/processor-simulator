#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "reorder_buffer.h"
#include "reg.h"

struct reorder_buffer *reorder_buffer_init(struct reg *rob_full)
{
    struct reorder_buffer *rob = malloc(sizeof(struct reorder_buffer));
    if (rob == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for reorder buffer");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < REORDER_BUFFER_SIZE; i++)
    {
        rob->entries_current[i].id = i;
        rob->entries_current[i].busy = false;
        rob->entries_current[i].ready = false;
        rob->entries_next[i].id = i;
        rob->entries_next[i].busy = false;
        rob->entries_next[i].ready = false;
    }

    rob->front_current = -1;
    rob->rear_current = -1;
    rob->front_next = -1;
    rob->rear_next = -1;
    rob->rob_full = rob_full;

    return rob;
}

void reorder_buffer_step(struct reorder_buffer *rob)
{
    bool is_full = (rob->rear_next + 1) % REORDER_BUFFER_SIZE == rob->front_next;
    reg_write(rob->rob_full, is_full);
}

void reorder_buffer_add() {}

void reorder_buffer_destroy(struct reorder_buffer *rob)
{
    free(rob);
}
