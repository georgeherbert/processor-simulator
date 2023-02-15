#ifndef EXECUTE_H
#define EXECUTE_H

struct execute_unit
{
};

struct execute_unit execute_init();                   // Initialise execute unit
void execute_step(struct execute_unit *execute_unit); // Step execute unit

#endif // EXECUTE_H
