#ifndef EXECUTE_H
#define EXECUTE_H

struct execute_unit
{
    int tmp;
};

struct execute_unit *execute_init();                  // Initialise execute unit
void execute_step(struct execute_unit *execute_unit); // Step execute unit
void execute_destroy(struct execute_unit *execute_unit);

#endif // EXECUTE_H
