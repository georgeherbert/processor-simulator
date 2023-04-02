#include <stdlib.h>
#include <stdio.h>
#include "commit.h"
#include "rob.h"
#include "decoded_inst.h"
#include "res_stations.h"
#include "memory_buffers.h"
#include "alu.h"
#include "inst_queue.h"
#include "reg_file.h"
#include "control.h"

struct commit_unit *commit_init(
    struct rob *rob,
    struct main_memory *mm,
    struct reg_file *reg_file,
    struct res_stations *alu_rs,
    struct res_stations *branch_rs,
    struct memory_buffers *mb,
    struct alu_unit *alu,
    struct inst_queue *iq,
    struct reg *inst_reg,
    struct reg *pc_src,
    struct reg *reg_pc_target,
    struct btb *btb,
    bool *jump_zero)
{
    struct commit_unit *commit_unit = malloc(sizeof(struct commit_unit));
    if (commit_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for commit unit");
        exit(EXIT_FAILURE);
    }

    commit_unit->rob = rob;
    commit_unit->mm = mm;
    commit_unit->reg_file = reg_file;
    commit_unit->alu_rs = alu_rs;
    commit_unit->branch_rs = branch_rs;
    commit_unit->mb = mb;
    commit_unit->alu = alu;
    commit_unit->iq = iq;
    commit_unit->inst_reg = inst_reg;
    commit_unit->pc_src = pc_src;
    commit_unit->reg_pc_target = reg_pc_target;
    commit_unit->btb = btb;
    commit_unit->jump_zero = jump_zero;

    return commit_unit;
}

void commit_clear(struct commit_unit *commit_unit)
{
    /*
        Incorrect branch prediction, we need to clear:
        - The ROB
        - ROB IDs in the register file
        - Reservation stations
        - ALU unit
        - Memory buffers
        - Instruction queue
        - Instruction register
    */
    rob_clear(commit_unit->rob);
    reg_file_clear(commit_unit->reg_file);
    res_stations_clear(commit_unit->alu_rs);
    res_stations_clear(commit_unit->branch_rs);
    memory_buffers_clear(commit_unit->mb);
    alu_clear(commit_unit->alu);
    inst_queue_clear(commit_unit->iq);
    reg_write(commit_unit->inst_reg, 0x0);
}

bool commit_step(struct commit_unit *commit_unit)
{
    if (rob_ready(commit_unit->rob))
    {
        struct rob_entry entry = rob_dequeue(commit_unit->rob);
        switch (entry.op_type)
        {
        case JUMP:
            printf("\tCommit: Jump (%d)\n", entry.inst_pc);
            reg_file_reg_commit(commit_unit->reg_file, entry.dest, entry.value, entry.rob_id);
            if (entry.npc_actual != entry.npc_pred)
            {
                // printf("J: %d %d\n", entry.npc_actual, entry.npc_pred);
                btb_set(commit_unit->btb, entry.inst_pc, entry.npc_actual);
                commit_clear(commit_unit);
                reg_write(commit_unit->reg_pc_target, entry.npc_actual);
                reg_write(commit_unit->pc_src, PC_SRC_MISPREDICT);
            }
            else
            {
                // printf("Correct prediction\n");
            }
            *commit_unit->jump_zero = (entry.npc_actual == 0x0); // End of program
            break;
        case BRANCH:
            printf("\tCommit: Branch (%d) %d %d\n", entry.inst_pc, entry.npc_actual, entry.npc_pred);
            if (entry.npc_actual != entry.npc_pred)
            {
                // printf("B: %d %d\n", entry.npc_actual, entry.npc_pred);
                btb_set(commit_unit->btb, entry.inst_pc, entry.npc_actual);
                commit_clear(commit_unit);
                reg_write(commit_unit->reg_pc_target, entry.npc_actual);
                reg_write(commit_unit->pc_src, PC_SRC_MISPREDICT);
            }
            else
            {
                // printf("Correct prediction\n");
            }
            break;
        case LOAD:
        case AL:
            printf("\tCommit: AL/Load %d %d\n", entry.dest, entry.value);
            reg_file_reg_commit(commit_unit->reg_file, entry.dest, entry.value, entry.rob_id);
            break;
        case STORE_WORD:
            printf("\tCommit: SW %d %d\n", entry.dest, entry.value);
            // printf("\t%d %d\n", entry.dest, entry.value);
            main_memory_store_word(commit_unit->mm, entry.dest, entry.value);
            break;
        case STORE_HALF:
            printf("\tCommit: SH %d %d\n", entry.dest, entry.value);
            // printf("\t%d %d\n", entry.dest, entry.value);
            main_memory_store_half(commit_unit->mm, entry.dest, entry.value);
            break;
        case STORE_BYTE:
            printf("\tCommit: SB %d %d\n", entry.dest, entry.value);
            // printf("\t%d %d\n", entry.dest, entry.value);
            main_memory_store_byte(commit_unit->mm, entry.dest, entry.value);
            break;
        default:
            fprintf(stderr, "Error: Invalid op type in commit step\n");
            exit(EXIT_FAILURE);
        }
        return true;
    }
    return false;
}

void commit_destroy(struct commit_unit *commit_unit)
{
    free(commit_unit);
}
