#include <stdbool.h>
#include "reorder_buffer.h"

bool reorder_buffer_is_not_full(struct reorder_buffer *rob)
{
    return (rob->rear + 1) % REORDER_BUFFER_SIZE != rob->front;
}

void reorder_buffer_add() {}
