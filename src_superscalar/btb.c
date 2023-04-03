#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "btb.h"
#include "config.h"

struct btb *btb_init()
{
    struct btb *btb = malloc(sizeof(struct btb));

    if (btb == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for BTB\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < BTB_SIZE; i++)
    {
        btb->entries[i].bits = 1; // Weakly not taken
    }

    return btb;
}

uint32_t btb_lookup(struct btb *btb, uint32_t pc)
{
    struct btb_entry btb_entry = btb->entries[(pc / 4) % BTB_SIZE];
    return btb_entry.bits > 1 ? btb_entry.npc_pred : pc + 4;
}

void btb_taken(struct btb *btb, uint32_t pc, uint32_t npc_pred)
{
    uint8_t *bits = &btb->entries[(pc / 4) % BTB_SIZE].bits;
    *bits = *bits == 3 ? 3 : *bits + 1;
    btb->entries[(pc / 4) % BTB_SIZE].npc_pred = npc_pred;
}

void btb_not_taken(struct btb *btb, uint32_t pc)
{
    uint8_t *bits = &btb->entries[(pc / 4) % BTB_SIZE].bits;
    *bits = *bits == 0 ? 0 : *bits - 1;
}

void btb_destroy(struct btb *btb)
{
    free(btb);
}
