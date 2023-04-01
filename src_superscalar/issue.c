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
#include "reorder_buffer.h"

#define NA 0

struct issue_unit *issue_init(
    struct inst_queue *inst_queue,
    struct reg_file *reg_file,
    struct res_stations *alu_res_stations,
    struct res_stations *branch_res_stations,
    struct memory_buffers *memory_buffers,
    struct reorder_buffer *rob,
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
    issue_unit->rob = rob;
    issue_unit->inst_queue_empty = inst_queue_empty;
    issue_unit->res_stations_all_busy_alu = res_stations_all_busy_alu;
    issue_unit->res_stations_all_busy_branch = res_stations_all_busy_branch;
    issue_unit->memory_buffers_all_busy = memory_buffers_all_busy;
    issue_unit->rob_full = rob_full;

    return issue_unit;
}

void set_q_v(struct reg_file *reg_file, struct reorder_buffer *rob, uint32_t rs_addr, uint32_t *q, uint32_t *v)
{
    if (reg_file_get_reg_busy(reg_file, rs_addr))
    {
        uint32_t rob_id = reg_file_get_rob_id(reg_file, rs_addr);
        if (reorder_buffer_is_entry_ready(rob, rob_id))
        {
            *q = NA;
            *v = reorder_buffer_get_entry_value(rob, rob_id);
        }
        else
        {
            *q = rob_id;
            *v = NA;
        }
    }
    else
    {
        *q = NA;
        *v = reg_file_get_reg_val(reg_file, rs_addr);
    }
}

void handle_al_operation(
    struct decoded_inst inst,
    struct reorder_buffer *rob,
    struct reg_file *reg_file,
    struct res_stations *alu_res_stations)
{
    uint32_t dest_rob_id = reorder_buffer_enqueue(rob, inst.op_type, inst.rd_addr, NA, NA);

    uint32_t qj = NA;
    uint32_t qk = NA;
    uint32_t vj = NA;
    uint32_t vk = NA;
    uint32_t inst_pc = NA;

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
        set_q_v(reg_file, rob, inst.rs1_addr, &qj, &vj);
        vk = inst.imm;
        break;
    case LUI:
        vj = inst.imm;
        break;
    case AUIPC:
        vj = inst.imm;
        inst_pc = inst.inst_pc;
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
        set_q_v(reg_file, rob, inst.rs1_addr, &qj, &vj);
        set_q_v(reg_file, rob, inst.rs2_addr, &qk, &vk);
        break;
    default:
        fprintf(stderr, "Error: Unknown arithmetic or logical op %d", inst.op);
        exit(EXIT_FAILURE);
        break;
    }

    res_stations_add(
        alu_res_stations,
        inst.op,
        qj,
        qk,
        vj,
        vk,
        NA,
        dest_rob_id,
        inst_pc);

    reg_file_set_rob_id(reg_file, inst.rd_addr, dest_rob_id);
}

void handle_branch_operation(
    struct decoded_inst inst,
    struct reorder_buffer *rob,
    struct reg_file *reg_file,
    struct res_stations *branch_res_stations)
{
    /*
        Branch instructions have no rd_addr.
        But rd_addr should be NA from the decode unit for branch instructions.
    */
    uint32_t dest_rob_id = reorder_buffer_enqueue(rob, inst.op_type, inst.rd_addr, NA, NA);

    uint32_t qj = NA;
    uint32_t qk = NA;
    uint32_t vj = NA;
    uint32_t vk = NA;

    switch (inst.op)
    {
    case JAL:
        reg_file_set_rob_id(reg_file, inst.rd_addr, dest_rob_id);
        break;
    case JALR:
        set_q_v(reg_file, rob, inst.rs1_addr, &qj, &vj);
        reg_file_set_rob_id(reg_file, inst.rd_addr, dest_rob_id);
        break;
    case BEQ:
    case BNE:
    case BLT:
    case BLTU:
    case BGE:
    case BGEU:
        set_q_v(reg_file, rob, inst.rs1_addr, &qj, &vj);
        set_q_v(reg_file, rob, inst.rs2_addr, &qk, &vk);
        // printf("vj: %d, qj: %d, vk: %d, qk: %d\n", vj, qj, vk, qk);
        break;
    default:
        fprintf(stderr, "Error: Unknown branch op");
        exit(EXIT_FAILURE);
        break;
    }

    res_stations_add(
        branch_res_stations,
        inst.op,
        qj,
        qk,
        vj,
        vk,
        inst.imm,
        dest_rob_id,
        inst.inst_pc);
}

void handle_mem_operation(
    struct decoded_inst inst,
    struct reorder_buffer *rob,
    struct reg_file *reg_file,
    struct memory_buffers *memory_buffers)
{
    /*
        Store instructions have no rd_addr.
        But rd_addr should be NA from the decode unit for store instructions.
    */

    uint32_t dest_rob_id;

    uint32_t qj = NA;
    uint32_t qk = NA;
    uint32_t vj = NA;
    uint32_t vk = NA;

    switch (inst.op)
    {
    case LW:
    case LH:
    case LHU:
    case LB:
    case LBU:
        set_q_v(reg_file, rob, inst.rs1_addr, &qj, &vj);
        dest_rob_id = reorder_buffer_enqueue(rob, inst.op_type, inst.rd_addr, NA, NA);
        reg_file_set_rob_id(reg_file, inst.rd_addr, dest_rob_id);
        break;
    case SW:
    case SH:
    case SB:
        set_q_v(reg_file, rob, inst.rs1_addr, &qj, &vj);
        set_q_v(reg_file, rob, inst.rs2_addr, &qk, &vk);
        dest_rob_id = reorder_buffer_enqueue(rob, inst.op_type, NA, vk, qk);
        break;
    default:
        fprintf(stderr, "Error: Unknown memory op");
        exit(EXIT_FAILURE);
        break;
    }

    memory_buffers_enqueue(
        memory_buffers,
        inst.op,
        qj,
        qk,
        vj,
        vk,
        inst.imm,
        dest_rob_id);
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
                // printf("\tIssue: AL\n");
                struct decoded_inst inst = inst_queue_dequeue(issue_unit->inst_queue);
                handle_al_operation(inst, issue_unit->rob, issue_unit->reg_file, issue_unit->alu_res_stations);
            }
            break;
        case JUMP:
        case BRANCH:
            if (!reg_read(issue_unit->res_stations_all_busy_branch))
            {
                // printf("\tIssue: JUMP/BRANCH\n");
                struct decoded_inst inst = inst_queue_dequeue(issue_unit->inst_queue);
                handle_branch_operation(inst, issue_unit->rob, issue_unit->reg_file, issue_unit->branch_res_stations);
            }
            break;
        case LOAD:
        case STORE_WORD:
        case STORE_HALF:
        case STORE_BYTE:
            if (!reg_read(issue_unit->memory_buffers_all_busy))
            {
                // printf("\tIssue: LOAD/STORE\n");
                struct decoded_inst inst = inst_queue_dequeue(issue_unit->inst_queue);
                handle_mem_operation(inst, issue_unit->rob, issue_unit->reg_file, issue_unit->memory_buffers);
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
