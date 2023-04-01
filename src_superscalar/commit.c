#include <stdlib.h>
#include <stdio.h>
#include "commit.h"
#include "rob.h"
#include "decoded_inst.h"
#include "reg_file.h"

struct commit_unit *commit_init(struct rob *rob, struct reg *rob_ready, struct main_memory *mm, struct reg_file *reg_file)
{
    struct commit_unit *commit_unit = malloc(sizeof(struct commit_unit));
    if (commit_unit == NULL)
    {
        fprintf(stderr, "Error: Could not allocate memory for commit unit");
        exit(EXIT_FAILURE);
    }

    commit_unit->rob = rob;
    commit_unit->rob_ready = rob_ready;
    commit_unit->mm = mm;
    commit_unit->reg_file = reg_file;

    return commit_unit;
}

bool commit_step(struct commit_unit *commit_unit)
{
    if (reg_read(commit_unit->rob_ready))
    {
        struct rob_entry entry = rob_dequeue(commit_unit->rob);
        switch (entry.op_type)
        {
        case JUMP:
            // printf("\tCommit: Jump %d %d\n", entry.dest, entry.value);
            reg_file_reg_commit(commit_unit->reg_file, entry.dest, entry.value, entry.rob_id);
            break; // TODO: Fix this
        case BRANCH:
            // printf("\tCommit: Branch\n");
            break; // TODO: Fix this
        case LOAD:
        case AL:
            // printf("\tCommit: Load %d %d\n", entry.dest, entry.value);
            reg_file_reg_commit(commit_unit->reg_file, entry.dest, entry.value, entry.rob_id);
            break;
        case STORE_WORD:
            // printf("\tCommit: SW %d %d\n", entry.dest, entry.value);
            // printf("\t%d %d\n", entry.dest, entry.value);
            main_memory_store_word(commit_unit->mm, entry.dest, entry.value);
            break;
        case STORE_HALF:
            // printf("\tCommit: SH %d %d\n", entry.dest, entry.value);
            // printf("\t%d %d\n", entry.dest, entry.value);
            main_memory_store_half(commit_unit->mm, entry.dest, entry.value);
            break;
        case STORE_BYTE:
            // printf("\tCommit: SB %d %d\n", entry.dest, entry.value);
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
