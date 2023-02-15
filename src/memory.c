#include <stdlib.h>
#include <stdio.h>
#include "memory.h"

#define MEMORY_SIZE 1024 * 1024 * 32 // 32MiB

uint8_t memory_load_byte(struct memory *memory, uint32_t address)
{
    return memory->bytes[address];
}

uint16_t memory_load_half(struct memory *memory, uint32_t address)
{
    uint16_t half = 0;
    half |= memory_load_byte(memory, address);
    half |= memory_load_byte(memory, address + 1) << 8;
    return half;
}

uint32_t memory_load_word(struct memory *memory, uint32_t address)
{
    uint32_t word = 0;
    word |= memory_load_half(memory, address);
    word |= memory_load_half(memory, address + 2) << 16;
    return word;
}

void memory_store_byte(struct memory *memory, uint32_t address, uint8_t value)
{
    memory->bytes[address] = value;
}

void memory_store_half(struct memory *memory, uint32_t address, uint16_t value)
{
    memory_store_byte(memory, address, value & 0xFF);
    memory_store_byte(memory, address + 1, value >> 8);
}

void memory_store_word(struct memory *memory, uint32_t address, uint32_t value)
{
    memory_store_half(memory, address, value & 0xFFFF);
    memory_store_half(memory, address + 2, value >> 16);
}

struct memory memory_init(char *file_name)
{
    FILE *fp = fopen(file_name, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Error opening memory file.\n");
        exit(EXIT_FAILURE);
    }

    struct memory memory;
    memory.bytes = calloc(MEMORY_SIZE, sizeof(uint8_t));
    if (memory.bytes == NULL)
    {
        fprintf(stderr, "Error allocating memory.\n");
        exit(EXIT_FAILURE);
    }

    size_t bytes_read = fread(memory.bytes, 1, MEMORY_SIZE, fp);
    if (bytes_read == 0)
    {
        fprintf(stderr, "Error reading from memory file.\n");
        exit(EXIT_FAILURE);
    }

    printf("memory initialised with %zu bytes from %s.\n", bytes_read, file_name);

    return memory;
}

void memory_destroy(struct memory *memory)
{
    free(memory->bytes);
}
