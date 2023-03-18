#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "issue.h"
#include "res_station.h"
#include "decode.h"

struct issue_unit *issue_init(
    struct inst_queue *inst_queue,
    uint32_t *regs,
    struct res_station *alu_res_station,
    struct res_station *branch_res_station,
    struct res_station *mem_res_station)
{
    struct issue_unit *issue_unit = malloc(sizeof(struct issue_unit));

    if (issue_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for issue unit\n");
        exit(EXIT_FAILURE);
    }

    issue_unit->inst_queue = inst_queue;
    issue_unit->regs = regs;
    issue_unit->alu_res_station = alu_res_station;
    issue_unit->branch_res_station = branch_res_station;
    issue_unit->mem_res_station = mem_res_station;

    return issue_unit;
}

void handle_al_operation(struct decoded_inst inst, uint32_t *regs, struct res_station *alu_res_station)
{
    switch (inst.op) {
        case ADDI:
        case SLTI:
        case SLTIU:
        case ANDI:
        case ORI:
        case XORI:
        case SLLI:
        case SRLI:
        case SRAI:
            res_station_add(alu_res_station, inst.op, regs[inst.rs1_addr], inst.imm, 0, inst.rd_addr, inst.inst_pc);
            break;
        case LUI:
            res_station_add(alu_res_station, inst.op, 0, inst.imm, 0, inst.rd_addr, inst.inst_pc);
            break;
        case AUIPC:
            res_station_add(alu_res_station, inst.op, inst.inst_pc, inst.imm, 0, inst.rd_addr, inst.inst_pc);
            break;
        case ADD:
        case SLT:
        case SLTU:
        case AND:
        case OR:
        case XOR:
        case SLL:
        case SRL:
        case SUB:
        case SRA:
            res_station_add(alu_res_station, inst.op, regs[inst.rs1_addr], regs[inst.rs2_addr], 0, inst.rd_addr, inst.inst_pc);
            break;
        default:
            fprintf(stderr, "Error: Unknown arithmetic or logical op");
            exit(EXIT_FAILURE);
            break;
    }
}

void handle_branch_operation(struct decoded_inst inst, uint32_t *regs, struct res_station *branch_res_station)
{
    switch (inst.op) {
        case JAL:
            // TODO: Decide what we're doing with the PC here
            res_station_add(branch_res_station, inst.op, 0, 0, inst.imm, inst.rd_addr, inst.inst_pc);
            break;
        case JALR:
            res_station_add(branch_res_station, inst.op, regs[inst.rs1_addr], 0, inst.imm, inst.rd_addr, inst.inst_pc);
            break;
        case BEQ:
        case BNE:
        case BLT:
        case BLTU:
        case BGE:
        case BGEU:
            // TODO: Decide what to do about the destination register
            res_station_add(branch_res_station, inst.op, regs[inst.rs1_addr], regs[inst.rs2_addr], inst.imm, inst.rd_addr, inst.inst_pc);
            break;
        default:
            fprintf(stderr, "Error: Unknown branch op");
            exit(EXIT_FAILURE);
            break;
    }
}

void handle_mem_operation(struct decoded_inst inst, uint32_t *regs, struct res_station *mem_res_station)
{
    // TODO: Decide what to do about the destination register for stores here
    switch (inst.op) {
        case LW:
        case LH:
        case LHU:
        case LB:
        case LBU:
            res_station_add(mem_res_station, inst.op, regs[inst.rs1_addr], 0, inst.imm, inst.rd_addr, inst.inst_pc);
            break;
        case SW:
        case SH:
        case SB:
            res_station_add(mem_res_station, inst.op, regs[inst.rs1_addr], regs[inst.rs2_addr], inst.imm, inst.rd_addr, inst.inst_pc);
            break;
        default:
            fprintf(stderr, "Error: Unknown memory op");
            exit(EXIT_FAILURE);
            break;
    }
}

void issue_step(struct issue_unit *issue_unit)
{
    struct decoded_inst inst = inst_queue_pop(issue_unit->inst_queue);
    switch (inst.op_type)
    {
    case AL:
        handle_al_operation(inst, issue_unit->regs, issue_unit->alu_res_station);
        break;
    case BRANCH:
        handle_branch_operation(inst, issue_unit->regs, issue_unit->branch_res_station);
        break;
    case MEMORY:
        handle_mem_operation(inst, issue_unit->regs, issue_unit->mem_res_station);
        break;
    default:
        fprintf(stderr, "Error: Unknown instruction type");
        exit(EXIT_FAILURE);
        break;
    }
}

void issue_destroy(struct issue_unit *issue_unit)
{
    free(issue_unit);
}
