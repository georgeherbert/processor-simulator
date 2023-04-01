#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "reorder_buffer.h"
#include "reg.h"
#include "decoded_inst.h"
#include "com_data_bus.h"

struct reorder_buffer *reorder_buffer_init(struct reg *rob_full, struct com_data_bus *cdb, struct reg *rob_ready)
{
    struct reorder_buffer *rob = malloc(sizeof(struct reorder_buffer));
    if (rob == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for reorder buffer");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < REORDER_BUFFER_SIZE; i++)
    {
        rob->queue_current[i].rob_id = i + 1; // TODO: Tidy this up
        rob->queue_next[i].rob_id = i + 1;
        rob->queue_current[i].ready = false;
        rob->queue_next[i].ready = false;
        rob->queue_current[i].busy = false;
        rob->queue_next[i].busy = false;
    }

    rob->front_current = -1;
    rob->front_next = -1;
    rob->rear_next = -1;
    rob->rob_full = rob_full;
    rob->cdb = cdb;
    rob->rob_ready = rob_ready;

    return rob;
}

void reorder_buffer_step(struct reorder_buffer *rob)
{
    for (uint32_t i = 0; i < REORDER_BUFFER_SIZE; i++)
    {
        if (rob->queue_current[i].busy && !rob->queue_current[i].ready)
        {
            if (com_data_bus_is_val_ready(rob->cdb, i + 1))
            {
                rob->queue_next[i].value = com_data_bus_get_val(rob->cdb, i + 1);
                rob->queue_next[i].ready = true;
            }

            enum op_type op_type = rob->queue_current[i].op_type;
            if ((op_type == STORE_WORD || op_type == STORE_HALF || op_type == STORE_BYTE) && rob->queue_current[i].q != 0)
            {
                if (com_data_bus_is_val_ready(rob->cdb, rob->queue_current[i].q))
                {
                    rob->queue_next[i].value = com_data_bus_get_val(rob->cdb, rob->queue_current[i].q);
                    rob->queue_next[i].q = 0;
                    if (rob->queue_next[i].dest != 0) // TODO: Can we never store to zero then?
                    {
                        rob->queue_next[i].ready = true;
                    }
                }
            }
        }
    }

    reg_write(rob->rob_full, (rob->rear_next + 1) % REORDER_BUFFER_SIZE == rob->front_next);
    reg_write(rob->rob_ready, rob->front_next != -1 && rob->queue_next[rob->front_next].ready);
}

uint32_t reorder_buffer_enqueue(struct reorder_buffer *rob, enum op_type op_type, uint32_t dest, uint32_t value, uint32_t q)
{
    if (rob->front_next == -1)
    {
        rob->front_next = 0;
    }
    rob->rear_next = (rob->rear_next + 1) % REORDER_BUFFER_SIZE;
    rob->queue_next[rob->rear_next].op_type = op_type;
    rob->queue_next[rob->rear_next].dest = dest;
    rob->queue_next[rob->rear_next].ready = false;
    rob->queue_next[rob->rear_next].busy = true;
    rob->queue_next[rob->rear_next].value = value;
    rob->queue_next[rob->rear_next].q = q;

    // We add 1 to the ID since 0 is reserved to indicate the value is ready
    return rob->rear_next + 1;
}

struct reorder_buffer_entry reorder_buffer_dequeue(struct reorder_buffer *rob)
{
    struct reorder_buffer_entry entry = rob->queue_current[rob->front_current];
    rob->queue_next[rob->front_current].busy = false;
    if (rob->front_next == rob->rear_next)
    {
        rob->front_next = -1;
        rob->rear_next = -1;
    }
    else
    {
        rob->front_next = (rob->front_next + 1) % REORDER_BUFFER_SIZE;
    }
    return entry;
}

bool reorder_buffer_is_entry_ready(struct reorder_buffer *rob, uint32_t id)
{
    return rob->queue_current[id - 1].ready;
}

uint32_t reorder_buffer_get_entry_value(struct reorder_buffer *rob, uint32_t id)
{
    return rob->queue_current[id - 1].value;
}

bool reorder_buffer_earlier_stores(struct reorder_buffer *rob, uint32_t rob_id, uint32_t addr)
{
    int32_t current_index = rob_id - 1;
    bool earlier_store = false;
    do
    {
        enum op_type op_type = rob->queue_current[current_index].op_type;
        earlier_store |= (op_type == STORE_WORD || op_type == STORE_HALF || op_type == STORE_BYTE) && (rob->queue_current[current_index].dest == addr) && rob->queue_current[current_index].busy;
        current_index = (current_index - 1 + REORDER_BUFFER_SIZE) % REORDER_BUFFER_SIZE;
    } while (current_index != rob->front_current && !earlier_store);
    return earlier_store;
}

void reorder_buffer_add_address(struct reorder_buffer *rob, uint32_t rob_id, uint32_t addr)
{
    rob->queue_next[rob_id - 1].dest = addr;
    if (rob->queue_next[rob_id - 1].q == 0)
    {
        rob->queue_next[rob_id - 1].ready = true;
    }
}

void reorder_buffer_update_current(struct reorder_buffer *rob)
{
    memcpy(rob->queue_current, rob->queue_next, sizeof(struct reorder_buffer_entry) * REORDER_BUFFER_SIZE);
    rob->front_current = rob->front_next;
}

void reorder_buffer_destroy(struct reorder_buffer *rob)
{
    free(rob);
}
