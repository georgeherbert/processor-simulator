#include <stdlib.h>
#include <stdio.h>
#include "address.h"
#include "reg.h"
#include "memory_buffers.h"

struct address_unit *address_init(struct memory_buffers *memory_buffers, struct reg *memory_buffers_ready_address)
{
    struct address_unit *address_unit = malloc(sizeof(struct address_unit));
    if (address_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for address");
        exit(EXIT_FAILURE);
    }

    address_unit->memory_buffers = memory_buffers;
    address_unit->memory_buffers_ready_address = memory_buffers_ready_address;

    return address_unit;
}

void address_step(struct address_unit *address_unit)
{
    if (reg_read(address_unit->memory_buffers_ready_address))
    {
        struct memory_buffer entry = memory_buffers_dequeue(address_unit->memory_buffers);
        uint32_t address = entry.vj + entry.a;
        memory_buffers_add_address(address_unit->memory_buffers, entry.id, address);
    }
}

void address_destroy(struct address_unit *address_unit)
{
    free(address_unit);
}
