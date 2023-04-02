#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "btb.h"

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
        btb->entries[i].valid = false;
    }

    return btb;
}

uint32_t btb_lookup(struct btb *btb, uint32_t pc)
{
    struct btb_entry btb_entry = btb->entries[(pc / 4) % BTB_SIZE];
    return btb_entry.valid ? btb_entry.npc_pred : pc + 4;
}

void btb_set(struct btb *btb, uint32_t pc, uint32_t npc_pred)
{
    btb->entries[(pc / 4) % BTB_SIZE].valid = true;
    btb->entries[(pc / 4) % BTB_SIZE].npc_pred = npc_pred;
}
