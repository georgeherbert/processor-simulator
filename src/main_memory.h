#ifndef MAIN_MEMORY_H
#define MAIN_MEMORY_H

#include <stdint.h>

#define MEMORY_SIZE 1024 * 1024 * 32 // 32MiB

struct main_memory
{
    uint8_t bytes[MEMORY_SIZE]; // Bytes in memory
};

uint8_t main_memory_load_byte(struct main_memory *mm, uint32_t addr);             // Load a byte from mem at the given addres
uint16_t main_memory_load_half(struct main_memory *mm, uint32_t addr);            // Load a half word from memory at the given addres
uint32_t main_memory_load_word(struct main_memory *mm, uint32_t addr);            // Load a word from memory at the given addres
void main_memory_store_byte(struct main_memory *mm, uint32_t addr, uint8_t val);  // Store a byte in memory at the given addres
void main_memory_store_half(struct main_memory *mm, uint32_t addr, uint16_t val); // Store a half word in memory at the given addres
void main_memory_store_word(struct main_memory *mm, uint32_t addr, uint32_t val); // Store a word in memory at the given addres
struct main_memory *main_memory_init(char *file_name);                            // Initialise memory from a file
void main_memory_destroy(struct main_memory *mm);                                 // Free memory

#endif // MAIN_MEMORY_H
