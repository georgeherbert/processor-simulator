#ifndef COMMIT_H
#define COMMIT_H

#include "reorder_buffer.h"
#include "reg.h"
#include "main_memory.h"

struct commit_unit
{
    struct reorder_buffer *rob;
    struct reg *rob_ready;
    struct main_memory *mm;
    struct reg_file *reg_file;
};

struct commit_unit *commit_init(struct reorder_buffer *rob, struct reg *rob_ready, struct main_memory *mm, struct reg_file *reg_file);
bool commit_step(struct commit_unit *commit_unit);
void commit_destroy(struct commit_unit *commit_unit);

#endif // COMMIT_H
