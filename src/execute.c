#include <stdlib.h>
#include <stdio.h>
#include "execute.h"

struct execute_unit *execute_init()
{
    struct execute_unit *execute_unit = malloc(sizeof(struct execute_unit));
    if (execute_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for execute unit\n");
        exit(EXIT_FAILURE);
    }

    execute_unit->tmp = 0;

    return execute_unit;
}

void execute_step(struct execute_unit *execute_unit)
{
    int tmp = 0;
    tmp += 1;
}

void execute_destroy(struct execute_unit *execute_unit)
{
    free(execute_unit);
}
