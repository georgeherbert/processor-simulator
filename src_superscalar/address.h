#ifndef ADDRESS_H
#define ADDRESS_H

#include "reg.h"
#include "memory_buffers.h"
#include "rob.h"

struct address_unit
{
    struct memory_buffers *memory_buffers; // Pointer to memory buffers
    struct rob *rob;                       // Pointer to reorder buffer
};

struct address_unit *address_init(struct memory_buffers *memory_buffers, struct rob *rob); // Initialise address unit
void address_step(struct address_unit *address_unit);                                      // Step address unit
void address_destroy(struct address_unit *address_unit);                                   // Free address unit

#endif // ADDRESS_H
