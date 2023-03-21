#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "issue.h"
#include "inst_queue.h"
#include "res_stations.h"
#include "reg_file.h"
#include "decode.h"
#include "decoded_inst.h"

#define NA 0

struct issue_unit *issue_init(
    struct inst_queue *inst_queue,
    struct reg_file *reg_file,
    struct res_stations *alu_res_stations,
    struct res_stations *branch_res_stations,
    struct res_stations *memory_res_stations)
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
    issue_unit->memory_res_stations = memory_res_stations;

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
            inst.inst_pc);
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
            inst.inst_pc);
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
            inst.inst_pc);
        break;
    default:
        fprintf(stderr, "Error: Unknown arithmetic or logical op");
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
        // TODO: Decide what to do about the destination register
        res_stations_add(
            branch_res_stations,
            inst.op,
            reg_file_get_reg_qi(reg_file, inst.rs1_addr),
            reg_file_get_reg_qi(reg_file, inst.rs2_addr),
            reg_file_get_reg_val_or_na(reg_file, inst.rs1_addr),
            reg_file_get_reg_val_or_na(reg_file, inst.rs2_addr),
            inst.imm,
            inst.rd_addr,
            inst.inst_pc);
        break;
    default:
        fprintf(stderr, "Error: Unknown branch op");
        exit(EXIT_FAILURE);
        break;
    }
}

void handle_mem_operation(struct decoded_inst inst, struct reg_file *reg_file, struct res_stations *memory_res_stations)
{
    // TODO: Decide what to do about the destination register for stores here
    switch (inst.op)
    {
    case LW:
    case LH:
    case LHU:
    case LB:
    case LBU:
        res_stations_add(
            memory_res_stations,
            inst.op,
            reg_file_get_reg_qi(reg_file, inst.rs1_addr),
            NA, // Load operations don't use rs2
            reg_file_get_reg_val_or_na(reg_file, inst.rs1_addr),
            NA, // Load operations only have base and offset operands
            inst.imm,
            inst.rd_addr,
            inst.inst_pc);
        break;
    case SW:
    case SH:
    case SB:
        res_stations_add(
            memory_res_stations,
            inst.op,
            reg_file_get_reg_qi(reg_file, inst.rs1_addr),
            reg_file_get_reg_qi(reg_file, inst.rs2_addr),
            reg_file_get_reg_val_or_na(reg_file, inst.rs1_addr),
            reg_file_get_reg_val_or_na(reg_file, inst.rs2_addr),
            inst.imm,
            inst.rd_addr,
            inst.inst_pc);
        break;
    default:
        fprintf(stderr, "Error: Unknown memory op");
        exit(EXIT_FAILURE);
        break;
    }
}

void issue_step(struct issue_unit *issue_unit)
{
    if (inst_queue_is_not_empty(issue_unit->inst_queue))
    {
        enum op_type op_type = inst_queue_peek_op_type(issue_unit->inst_queue);
        switch (op_type)
        {
        case AL:
            if (res_stations_not_full(issue_unit->alu_res_stations))
            {
                struct decoded_inst inst = inst_queue_dequeue(issue_unit->inst_queue);
                handle_al_operation(inst, issue_unit->reg_file, issue_unit->alu_res_stations);
            }
            break;
        case BRANCH:
            if (res_stations_not_full(issue_unit->branch_res_stations))
            {
                struct decoded_inst inst = inst_queue_dequeue(issue_unit->inst_queue);
                handle_branch_operation(inst, issue_unit->reg_file, issue_unit->branch_res_stations);
            }
            break;
        case MEMORY:
            if (res_stations_not_full(issue_unit->memory_res_stations))
            {
                struct decoded_inst inst = inst_queue_dequeue(issue_unit->inst_queue);
                handle_mem_operation(inst, issue_unit->reg_file, issue_unit->memory_res_stations);
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
