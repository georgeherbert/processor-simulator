#include <stdlib.h>
#include <stdio.h>
#include "address.h"

struct address_unit *address_init()
{
    struct address_unit *address_unit = malloc(sizeof(struct address_unit));
    if (address_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for address");
        exit(EXIT_FAILURE);
    }

    return address_unit;
}

void address_step(struct address_unit *address_unit)
{
    return;
}

void address_destroy(struct address_unit *address_unit)
{
    free(address_unit);
}
