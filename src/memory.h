#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

struct memory
{
    uint8_t *bytes; // Bytes in memory
};

uint8_t memory_load_byte(struct memory *memory, uint32_t addres);               // Load a byte from mem at the given address
uint16_t memory_load_half(struct memory *memory, uint32_t addres);              // Load a half word from memory at the given address
uint32_t memory_load_word(struct memory *memory, uint32_t addres);              // Load a word from memory at the given address
void memory_store_byte(struct memory *memory, uint32_t addres, uint8_t value);  // Store a byte in memory at the given address
void memory_store_half(struct memory *memory, uint32_t addres, uint16_t value); // Store a half word in memory at the given address
void memory_store_word(struct memory *memory, uint32_t addres, uint32_t value); // Store a word in memory at the given address
struct memory memory_init(char *file_name);                                     // Initialise memory from a file
void memory_destroy(struct memory *memory);                                     // Free memory

#endif // MEMORY_H
