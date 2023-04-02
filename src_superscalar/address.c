#include <stdlib.h>
#include <stdio.h>
#include "address.h"
#include "reg.h"
#include "memory_buffers.h"
#include "rob.h"

struct address_unit *address_init(struct memory_buffers *memory_buffers, struct rob *rob)
{
    struct address_unit *address_unit = malloc(sizeof(struct address_unit));
    if (address_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for address");
        exit(EXIT_FAILURE);
    }

    address_unit->memory_buffers = memory_buffers;
    address_unit->rob = rob;

    return address_unit;
}

void address_step(struct address_unit *address_unit)
{
    struct memory_buffer *mb_entry = memory_buffers_dequeue_address(address_unit->memory_buffers);

    if (mb_entry)
    {
        uint32_t address = mb_entry->vj + mb_entry->a;
        if (mb_entry->op == LW || mb_entry->op == LH || mb_entry->op == LHU || mb_entry->op == LB || mb_entry->op == LBU)
        {
            memory_buffers_add_address(address_unit->memory_buffers, mb_entry->id, address);
        }
        else if (mb_entry->op == SW || mb_entry->op == SH || mb_entry->op == SB)
        {
            rob_add_address(address_unit->rob, mb_entry->rob_id, address);
        }
    }
}

void address_destroy(struct address_unit *address_unit)
{
    free(address_unit);
}
