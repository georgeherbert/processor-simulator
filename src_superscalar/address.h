#ifndef ADDRESS_H
#define ADDRESS_H

struct address_unit
{
    int placeholder;
};

struct address_unit *address_init();                     // Initialise address unit
void address_step(struct address_unit *address_unit);    // Step address unit
void address_destroy(struct address_unit *address_unit); // Free address unit

#endif // ADDRESS_H
