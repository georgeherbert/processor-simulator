#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "issue.h"
#include "inst_queue.h"
#include "res_stations.h"
#include "reg_file.h"
#include "decode.h"
#include "decoded_inst.h"
#include "memory_buffers.h"

#define NA 0

struct issue_unit *issue_init(
    struct inst_queue *inst_queue,
    struct reg_file *reg_file,
    struct res_stations *alu_res_stations,
    struct res_stations *branch_res_stations,
    struct memory_buffers *memory_buffers,
    struct reg *inst_queue_empty,
    struct reg *res_stations_all_busy_alu,
    struct reg *res_stations_all_busy_branch,
    struct reg *memory_buffers_all_busy,
    struct reg *rob_full)
{
    struct issue_unit *issue_unit = malloc(sizeof(struct issue_unit));

    if (issue_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for issue unit\n");
        exit(EXIT_FAILURE);
    }

    issue_unit->inst_queue = inst_queue;
    issue_unit->reg_file = reg_file;
    issue_unit->alu_res_stations = alu_res_stations;
    issue_unit->branch_res_stations = branch_res_stations;
    issue_unit->memory_buffers = memory_buffers;
    issue_unit->inst_queue_empty = inst_queue_empty;
    issue_unit->res_stations_all_busy_alu = res_stations_all_busy_alu;
    issue_unit->res_stations_all_busy_branch = res_stations_all_busy_branch;
    issue_unit->memory_buffers_all_busy = memory_buffers_all_busy;
    issue_unit->rob_full = rob_full;

    return issue_unit;
}

void handle_al_operation(struct decoded_inst inst, struct reg_file *reg_file, struct res_stations *alu_res_stations)
{
    switch (inst.op)
    {
    case ADDI:
    case SLTI:
    case SLTIU:
    case ANDI:
    case ORI:
    case XORI:
    case SLLI:
    case SRLI:
    case SRAI:
        res_stations_add(
            alu_res_stations,
            inst.op,
            reg_file_get_reg_qi(reg_file, inst.rs1_addr),
            NA, // Integer register-immediate operations don't use rs2
            reg_file_get_reg_val_or_na(reg_file, inst.rs1_addr),
            inst.imm,
            NA, // Only used in branch and memory instructions
            inst.rd_addr,
            NA); // Only used in branches and AUIPC
        break;
    case LUI:
        res_stations_add(
            alu_res_stations,
            inst.op,
            NA, // LUI doesn't use rs1
            NA, // LUI doesn't use rs2
            inst.imm,
            NA, // LUI only has one operand
            NA, // Only used in branch and memory instructions
            inst.rd_addr,
            NA); // Only used in branches and AUIPC
        break;
    case AUIPC:
        res_stations_add(
            alu_res_stations,
            inst.op,
            NA, // AUIPC doesn't use rs1
            NA, // AUIPC doesn't use rs2
            inst.imm,
            NA, // AUIPC only has one operand
            NA, // Only used in branch and memory instructions
            inst.rd_addr,
            inst.inst_pc);
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
        res_stations_add(
            alu_res_stations,
            inst.op,
            reg_file_get_reg_qi(reg_file, inst.rs1_addr),
            reg_file_get_reg_qi(reg_file, inst.rs2_addr),
            reg_file_get_reg_val_or_na(reg_file, inst.rs1_addr),
            reg_file_get_reg_val_or_na(reg_file, inst.rs2_addr),
            NA, // Only used in branch and memory instructions
            inst.rd_addr,
            NA); // Only used in branches and AUIPC
        break;
    default:
        fprintf(stderr, "Error: Unknown arithmetic or logical op %d", inst.op);
        exit(EXIT_FAILURE);
        break;
    }
}

void handle_branch_operation(struct decoded_inst inst, struct reg_file *reg_file, struct res_stations *branch_res_stations)
{
    switch (inst.op)
    {
    case JAL:
        res_stations_add(
            branch_res_stations,
            inst.op,
            NA, // JAL doesn't use rs1
            NA, // JAL doesn't use rs2
            NA, // JAL only has an offset operand
            NA, // JAL only has an offset operand
            inst.imm,
            inst.rd_addr,
            inst.inst_pc);
        break;
    case JALR:
        res_stations_add(
            branch_res_stations,
            inst.op,
            reg_file_get_reg_qi(reg_file, inst.rs1_addr),
            NA, // JALR doesn't use rs2
            reg_file_get_reg_val_or_na(reg_file, inst.rs1_addr),
            NA, // JALR only has base and offset operands
            inst.imm,
            inst.rd_addr,
            inst.inst_pc);
        break;
    case BEQ:
    case BNE:
    case BLT:
    case BLTU:
    case BGE:
    case BGEU:
        res_stations_add(
            branch_res_stations,
            inst.op,
            reg_file_get_reg_qi(reg_file, inst.rs1_addr),
            reg_file_get_reg_qi(reg_file, inst.rs2_addr),
            reg_file_get_reg_val_or_na(reg_file, inst.rs1_addr),
            reg_file_get_reg_val_or_na(reg_file, inst.rs2_addr),
            inst.imm,
            NA, // Branch operations don't write to a register
            inst.inst_pc);
        break;
    default:
        fprintf(stderr, "Error: Unknown branch op");
        exit(EXIT_FAILURE);
        break;
    }
}

void handle_mem_operation(struct decoded_inst inst, struct reg_file *reg_file, struct memory_buffers *memory_buffers)
{
    switch (inst.op)
    {
    case LW:
    case LH:
    case LHU:
    case LB:
    case LBU:
        memory_buffers_enqueue(
            memory_buffers,
            inst.op,
            reg_file_get_reg_qi(reg_file, inst.rs1_addr),
            NA, // Load operations don't use rs2
            reg_file_get_reg_val_or_na(reg_file, inst.rs1_addr),
            NA, // Load operations only have base and offset operands
            inst.imm,
            inst.rd_addr);
        break;
    case SW:
    case SH:
    case SB:
        memory_buffers_enqueue(
            memory_buffers,
            inst.op,
            reg_file_get_reg_qi(reg_file, inst.rs1_addr),
            reg_file_get_reg_qi(reg_file, inst.rs2_addr),
            reg_file_get_reg_val_or_na(reg_file, inst.rs1_addr),
            reg_file_get_reg_val_or_na(reg_file, inst.rs2_addr),
            inst.imm,
            NA); // Store operations don't write to a register
        break;
    default:
        fprintf(stderr, "Error: Unknown memory op");
        exit(EXIT_FAILURE);
        break;
    }
}

void issue_step(struct issue_unit *issue_unit)
{
    if (!reg_read(issue_unit->inst_queue_empty) && !reg_read(issue_unit->rob_full))
    {
        enum op_type op_type = inst_queue_peek_op_type(issue_unit->inst_queue);
        switch (op_type)
        {
        case AL:
            if (!reg_read(issue_unit->res_stations_all_busy_alu))
            {
                struct decoded_inst inst = inst_queue_dequeue(issue_unit->inst_queue);
                handle_al_operation(inst, issue_unit->reg_file, issue_unit->alu_res_stations);
            }
            break;
        case BRANCH:
        case JUMP:
            if (!reg_read(issue_unit->res_stations_all_busy_branch))
            {
                struct decoded_inst inst = inst_queue_dequeue(issue_unit->inst_queue);
                handle_branch_operation(inst, issue_unit->reg_file, issue_unit->branch_res_stations);
            }
            break;
        case LOAD:
        case STORE:
            if (!reg_read(issue_unit->memory_buffers_all_busy))
            {
                struct decoded_inst inst = inst_queue_dequeue(issue_unit->inst_queue);
                handle_mem_operation(inst, issue_unit->reg_file, issue_unit->memory_buffers);
            }
            break;
        default:
            fprintf(stderr, "Error: Unknown instruction type");
            exit(EXIT_FAILURE);
            break;
        }
    }
}

void issue_destroy(struct issue_unit *issue_unit)
{
    free(issue_unit);
}
