#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "writeback.h"
#include "control.h"

struct writeback_unit *writeback_init(
    uint32_t *regs,
    enum ctrl_wb *ctrl_wb,
    uint8_t *reg_rd_addr,
    uint32_t *reg_alu_out,
    uint32_t *reg_mdr,
    uint32_t *reg_npc)
{
    struct writeback_unit *wb = malloc(sizeof(struct writeback_unit));
    if (wb == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for writeback unit");
        exit(EXIT_FAILURE);
    }

    wb->regs = regs;
    wb->ctrl_wb = ctrl_wb;
    wb->reg_rd_addr = reg_rd_addr;
    wb->reg_alu_out = reg_alu_out;
    wb->reg_mdr = reg_mdr;
    wb->reg_npc = reg_npc;
    return wb;
}

void writeback_step(struct writeback_unit *wb)
{
    switch (*wb->ctrl_wb)
    {
    case CTRL_WB_NONE:
        break;
    case CTRL_WB_ALU_OUT:
        printf("\tRF[%d] = %d\n", *wb->reg_rd_addr, *wb->reg_alu_out);
        if (*wb->reg_rd_addr != 0)
        {
            wb->regs[*wb->reg_rd_addr] = *wb->reg_alu_out;
        }
        break;
    case CTRL_WB_MDR:
        printf("\tRF[%d] = %d\n", *wb->reg_rd_addr, *wb->reg_mdr);
        if (*wb->reg_rd_addr != 0)
        {
            wb->regs[*wb->reg_rd_addr] = *wb->reg_mdr;
        }
        break;
    case CTRL_WB_NPC:
        printf("\tRF[%d] = %d\n", *wb->reg_rd_addr, *wb->reg_npc);
        if (*wb->reg_rd_addr != 0)
        {
            wb->regs[*wb->reg_rd_addr] = *wb->reg_npc;
        }
        break;
    default:
        fprintf(stderr, "Error: Invalid writeback control signal");
    }
}

void writeback_destroy(struct writeback_unit *wb)
{
    free(wb);
}
