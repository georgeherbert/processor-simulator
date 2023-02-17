#ifndef WRITEBACK_H
#define WRITEBACK_H

#include <stdint.h>
#include "control.h"

struct writeback_unit
{
    uint32_t *regs;        // Pointer to General-purpose registers
    enum ctrl_wb *ctrl_wb; // Pointer to Control signal for writeback source (or lack of)
    uint8_t *reg_rd_addr;  // Pointer to rd address register
    uint32_t *reg_alu_out; // Pointer to ALU operation output register
    uint32_t *reg_mdr;     // Pointer to Memory data register
    uint32_t *reg_npc;     // Pointer to next program counter register
};

struct writeback_unit *writeback_init(
    uint32_t *regs,
    enum ctrl_wb *ctrl_wb,
    uint8_t *reg_rd_addr,
    uint32_t *reg_alu_out,
    uint32_t *reg_mdr,
    uint32_t *reg_npc);
void writeback_step(struct writeback_unit *wb);
void writeback_destroy(struct writeback_unit *wb);

#endif // WRITEBACK_H
