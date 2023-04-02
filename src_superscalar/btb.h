#ifndef BTB_H
#define BTB_H

#include <stdint.h>
#include <stdbool.h>

#define BTB_SIZE 128

struct btb_entry
{
    bool valid;
    uint32_t npc_pred;
};

struct btb
{
    struct btb_entry entries[BTB_SIZE];
};

struct btb *btb_init();
uint32_t btb_lookup(struct btb *btb, uint32_t addr);
void btb_set(struct btb *btb, uint32_t addr, uint32_t npc_pred);

#endif // BTB_H
