#ifndef RV32I_H
#define RV32I_H

// Opcodes

#define OPCODE_OP_IMM 0b0010011
#define OPCODE_LUI    0b0110111
#define OPCODE_AUIPC  0b0010111
#define OPCODE_OP     0b0110011
#define OPCODE_JAL    0b1101111
#define OPCODE_JALR   0b1100111
#define OPCODE_BRANCH 0b1100011
#define OPCODE_LOAD   0b0000011
#define OPCODE_STORE  0b0100011

// Funct3 OP-IMM

#define FUNCT3_ADDI      0b000
#define FUNCT3_SLTI      0b010
#define FUNCT3_SLTIU     0b011
#define FUNCT3_XORI      0b100
#define FUNCT3_ORI       0b110
#define FUNCT3_ANDI      0b111
#define FUNCT3_SLLI      0b001
#define FUNCT3_SRLI_SRAI 0b101

// Funct3 OP

#define FUNCT3_ADD_SUB 0b000
#define FUNCT3_SLL     0b001
#define FUNCT3_SLT     0b010
#define FUNCT3_SLTU    0b011
#define FUNCT3_XOR     0b100
#define FUNCT3_SRL_SRA 0b101
#define FUNCT3_OR      0b110
#define FUNCT3_AND     0b111

// Funct3 BRANCH

#define FUNCT3_BEQ  0b000
#define FUNCT3_BNE  0b001
#define FUNCT3_BLT  0b100
#define FUNCT3_BGE  0b101
#define FUNCT3_BLTU 0b110
#define FUNCT3_BGEU 0b111

// Funct3 LOAD

#define FUNCT3_LB  0b000
#define FUNCT3_LH  0b001
#define FUNCT3_LW  0b010
#define FUNCT3_LBU 0b100
#define FUNCT3_LHU 0b101

// Funct3 STORE

#define FUNCT3_SB 0b000
#define FUNCT3_SH 0b001
#define FUNCT3_SW 0b010

// Funct7 OP

#define FUNCT7_ADD_SRL 0b0000000
#define FUNCT7_SUB_SRA 0b0100000

#endif // RV32I_H
