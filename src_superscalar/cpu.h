#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "main_memory.h"
#include "reg_file.h"
#include "fetch.h"
#include "decode.h"
#include "inst_queue.h"
#include "res_stations.h"
#include "issue.h"
#include "alu.h"
#include "branch.h"
#include "memory.h"
#include "control.h"
#include "com_data_bus.h"

struct cpu
{
    struct main_memory *mm;                   // Pointer to main memory
    struct fetch_unit *fetch_unit;            // Pointer to fetch unit
    struct decode_unit *decode_unit;          // Pointer to decode unit
    struct inst_queue *inst_queue;            // Pointer to instruction queue
    struct res_stations *alu_res_stations;    // Pointer to ALU reservation station
    struct res_stations *branch_res_stations; // Pointer to branch reservation station
    struct res_stations *memory_res_stations; // Pointer to memory reservation station
    struct issue_unit *issue_unit;            // Pointer to issue unit
    struct alu_unit *alu_unit;                // Pointer to ALU unit
    struct branch_unit *branch_unit;          // Pointer to branch unit
    struct memory_unit *memory_unit;          // Pointer to memory unit
    struct com_data_bus *cdb;                 // Pointer to common data bus

    struct reg_file *reg_file;                  // Pointer to register file
    enum pc_src pc_src;                         // Control signal for PC source
    enum branch_in_pipeline branch_in_pipeline; // Control signal for presence of branch in pipeline
    uint32_t reg_pc;                            // Program counter
    uint32_t reg_npc;                           // Next program counter
    uint32_t reg_pc_target;                     // PC target from branch unit
    uint32_t reg_inst;                          // Instruction register
};

struct cpu *cpu_init(char *file_name); // Initialise cpu
void cpu_destroy(struct cpu *cpu);     // Destroy relevent parts of cpu

#endif // CPU_H
