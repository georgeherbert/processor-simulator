#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "rv32i.h"

#define MAX_FIELDS 4

typedef struct node
{
    struct node *next;
    char *line;
    char **words;
    uint8_t num_words;
} node;

node *create_node(char *line)
{
    node *new_node = malloc(sizeof(node));
    new_node->line = strdup(line);
    new_node->next = NULL;
    return new_node;
}

void append_node(node **head, node **current, node *new_node)
{
    if (*head == NULL)
    {
        *head = new_node;
    }
    else
    {
        (*current)->next = new_node;
    }
    *current = new_node;
}

node *asm_to_lines_list(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("Error: could not open file.\n");
        exit(EXIT_FAILURE);
    }

    node *head = NULL;
    node *current = NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1)
    {
        if (line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }
        if (line[0] != '\0')
        {
            node *new_node = create_node(line);
            append_node(&head, &current, new_node);
        }
    }

    free(line);
    fclose(fp);

    return head;
}

void lines_list_to_word_lists(node *head)
{
    node *current = head;

    while (current != NULL)
    {
        current->num_words = 0;
        current->words = malloc(MAX_FIELDS * sizeof(char *));

        char *word = strtok(current->line, " ");
        while (word != NULL)
        {
            current->words[current->num_words] = strdup(word);
            current->num_words++;

            if (current->num_words > MAX_FIELDS)
            {
                perror("Error: too many fields in line.\n");
                exit(EXIT_FAILURE);
            }
            word = strtok(NULL, " ");
        }

        current->words = realloc(current->words, current->num_words * sizeof(char *));
        free(word);
        free(current->line);

        current = current->next;
    }
}

uint32_t get_num_instructions(node *head)
{
    node *current = head;
    uint32_t num_instructions = 0;

    while (current != NULL)
    {
        num_instructions++;
        current = current->next;
    }

    return num_instructions;
}

uint32_t str_register_to_uint5_t(char *reg_name)
{
    if (reg_name[0] != 'x')
    {
        perror("Error: invalid register name.\n");
        exit(EXIT_FAILURE);
    }

    char *endptr;
    unsigned long result = strtoul(reg_name + 1, &endptr, 10);
    if (endptr == reg_name + 1)
    {
        perror("Error: No digits were found in register name.\n");
        exit(EXIT_FAILURE);
    }

    if (result > 31)
    {
        perror("Error: Register name out of range.\n");
        exit(EXIT_FAILURE);
    }

    return (uint32_t)result;
}

int32_t str_to_int32_t(char *str)
{
    char *endptr;
    long result = strtol(str, &endptr, 10);
    if (*endptr != '\0')
    {
        perror("Error: Not a valid integer string\n");
        exit(EXIT_FAILURE);
    }

    return (int32_t)result;
}

uint32_t str_imm_to_uintn_t(char *str, uint8_t n)
{
    int32_t imm = str_to_int32_t(str);
    if (imm < 0 || imm > (1 << n) - 1)
    {
        perror("Error: Immediate out of range\n");
        exit(EXIT_FAILURE);
    }

    return (uint32_t)imm;
}

uint32_t str_imm_to_intn_t(char *str, uint8_t n)
{
    int32_t imm = str_to_int32_t(str);
    if (imm < -(1 << (n - 1)) || imm > (1 << (n - 1)) - 1)
    {
        perror("Error: Immediate out of range\n");
        exit(EXIT_FAILURE);
    }

    return (uint32_t)(imm >= 0 ? imm : (1 << n) + imm);
}

void word_lists_to_machine_code(node *head, rv32i_instruction *machine_code)
{
    node *current = head;
    uint32_t i = 0;

    while (current != NULL)
    {
        char **fields = current->words;
        char *operation = fields[0];
        uint8_t num_fields = current->num_words;

        // Integer Register-Immediate
        if (strcmp(operation, "addi") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = OP_IMM_OPCODE;
            machine_code[i].i_type.funct3 = ADDI_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "slti") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = OP_IMM_OPCODE;
            machine_code[i].i_type.funct3 = SLTI_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "sltiu") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = OP_IMM_OPCODE;
            machine_code[i].i_type.funct3 = SLTIU_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_uintn_t(fields[3], 12);
        }
        else if (strcmp(operation, "andi") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = OP_IMM_OPCODE;
            machine_code[i].i_type.funct3 = ANDI_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "ori") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = OP_IMM_OPCODE;
            machine_code[i].i_type.funct3 = ORI_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "xori") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = OP_IMM_OPCODE;
            machine_code[i].i_type.funct3 = XORI_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "slli") == 0 && num_fields == 4)
        {
            machine_code[i].i_type_shift.opcode = OP_IMM_OPCODE;
            machine_code[i].i_type_shift.funct3 = SLLI_FUNCT3;
            machine_code[i].i_type_shift.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type_shift.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type_shift.shamt = str_imm_to_uintn_t(fields[3], 5);
            machine_code[i].i_type_shift.shift_type = SLLI_SHIFT_TYPE;
        }
        else if (strcmp(operation, "srli") == 0 && num_fields == 4)
        {
            machine_code[i].i_type_shift.opcode = OP_IMM_OPCODE;
            machine_code[i].i_type_shift.funct3 = SRLI_FUNCT3;
            machine_code[i].i_type_shift.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type_shift.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type_shift.shamt = str_imm_to_uintn_t(fields[3], 5);
            machine_code[i].i_type_shift.shift_type = SRLI_SHIFT_TYPE;
        }
        else if (strcmp(operation, "srai") == 0 && num_fields == 4)
        {
            machine_code[i].i_type_shift.opcode = OP_IMM_OPCODE;
            machine_code[i].i_type_shift.funct3 = SRAI_FUNCT3;
            machine_code[i].i_type_shift.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type_shift.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type_shift.shamt = str_imm_to_uintn_t(fields[3], 5);
            machine_code[i].i_type_shift.shift_type = SRAI_SHIFT_TYPE;
        }
        else if (strcmp(operation, "lui") == 0 && num_fields == 3)
        {
            machine_code[i].u_type.opcode = LUI_OPCODE;
            machine_code[i].u_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].u_type.imm = str_imm_to_uintn_t(fields[2], 20);
        }
        else if (strcmp(operation, "auipc") == 0 && num_fields == 3)
        {
            machine_code[i].u_type.opcode = AUIPC_OPCODE;
            machine_code[i].u_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].u_type.imm = str_imm_to_uintn_t(fields[2], 20);
        }

        // Integer Register-Register
        else if (strcmp(operation, "add") == 0 && num_fields == 4)
        {
            machine_code[i].r_type.opcode = OP_OPCODE;
            machine_code[i].r_type.funct3 = ADD_FUNCT3;
            machine_code[i].r_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].r_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].r_type.rs2 = str_register_to_uint5_t(fields[3]);
            machine_code[i].r_type.funct7 = ADD_FUNCT7;
        }
        else if (strcmp(operation, "slt") == 0 && num_fields == 4)
        {
            machine_code[i].r_type.opcode = OP_OPCODE;
            machine_code[i].r_type.funct3 = SLT_FUNCT3;
            machine_code[i].r_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].r_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].r_type.rs2 = str_register_to_uint5_t(fields[3]);
            machine_code[i].r_type.funct7 = SLT_FUNCT7;
        }
        else if (strcmp(operation, "sltu") == 0 && num_fields == 4)
        {
            machine_code[i].r_type.opcode = OP_OPCODE;
            machine_code[i].r_type.funct3 = SLTU_FUNCT3;
            machine_code[i].r_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].r_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].r_type.rs2 = str_register_to_uint5_t(fields[3]);
            machine_code[i].r_type.funct7 = SLTU_FUNCT7;
        }
        else if (strcmp(operation, "and") == 0 && num_fields == 4)
        {
            machine_code[i].r_type.opcode = OP_OPCODE;
            machine_code[i].r_type.funct3 = AND_FUNCT3;
            machine_code[i].r_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].r_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].r_type.rs2 = str_register_to_uint5_t(fields[3]);
            machine_code[i].r_type.funct7 = AND_FUNCT7;
        }
        else if (strcmp(operation, "or") == 0 && num_fields == 4)
        {
            machine_code[i].r_type.opcode = OP_OPCODE;
            machine_code[i].r_type.funct3 = OR_FUNCT3;
            machine_code[i].r_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].r_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].r_type.rs2 = str_register_to_uint5_t(fields[3]);
            machine_code[i].r_type.funct7 = OR_FUNCT7;
        }
        else if (strcmp(operation, "xor") == 0 && num_fields == 4)
        {
            machine_code[i].r_type.opcode = OP_OPCODE;
            machine_code[i].r_type.funct3 = XOR_FUNCT3;
            machine_code[i].r_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].r_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].r_type.rs2 = str_register_to_uint5_t(fields[3]);
            machine_code[i].r_type.funct7 = XOR_FUNCT7;
        }
        else if (strcmp(operation, "sll") == 0 && num_fields == 4)
        {
            machine_code[i].r_type.opcode = OP_OPCODE;
            machine_code[i].r_type.funct3 = SLL_FUNCT3;
            machine_code[i].r_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].r_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].r_type.rs2 = str_register_to_uint5_t(fields[3]);
            machine_code[i].r_type.funct7 = SLL_FUNCT7;
        }
        else if (strcmp(operation, "srl") == 0 && num_fields == 4)
        {
            machine_code[i].r_type.opcode = OP_OPCODE;
            machine_code[i].r_type.funct3 = SRL_FUNCT3;
            machine_code[i].r_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].r_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].r_type.rs2 = str_register_to_uint5_t(fields[3]);
            machine_code[i].r_type.funct7 = SRL_FUNCT7;
        }
        else if (strcmp(operation, "sub") == 0 && num_fields == 4)
        {
            machine_code[i].r_type.opcode = OP_OPCODE;
            machine_code[i].r_type.funct3 = SUB_FUNCT3;
            machine_code[i].r_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].r_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].r_type.rs2 = str_register_to_uint5_t(fields[3]);
            machine_code[i].r_type.funct7 = SUB_FUNCT7;
        }
        else if (strcmp(operation, "sra") == 0 && num_fields == 4)
        {
            machine_code[i].r_type.opcode = OP_OPCODE;
            machine_code[i].r_type.funct3 = SRA_FUNCT3;
            machine_code[i].r_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].r_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].r_type.rs2 = str_register_to_uint5_t(fields[3]);
            machine_code[i].r_type.funct7 = SRA_FUNCT7;
        }

        // Unconditional jumps
        else if (strcmp(operation, "jal") == 0 && num_fields == 3)
        {
            machine_code[i].u_type.opcode = JAL_OPCODE;
            machine_code[i].u_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].u_type.imm = str_imm_to_intn_t(fields[2], 20);
        }
        else if (strcmp(operation, "jalr") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = JALR_OPCODE;
            machine_code[i].i_type.funct3 = JALR_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }

        // Conditional branches
        else if (strcmp(operation, "beq") == 0 && num_fields == 4)
        {
            machine_code[i].s_type.opcode = BRANCH_OPCODE;
            machine_code[i].s_type.funct3 = BEQ_FUNCT3;
            machine_code[i].s_type.rs1 = str_register_to_uint5_t(fields[1]);
            machine_code[i].s_type.rs2 = str_register_to_uint5_t(fields[2]);
            machine_code[i].s_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "bne") == 0 && num_fields == 4)
        {
            machine_code[i].s_type.opcode = BRANCH_OPCODE;
            machine_code[i].s_type.funct3 = BNE_FUNCT3;
            machine_code[i].s_type.rs1 = str_register_to_uint5_t(fields[1]);
            machine_code[i].s_type.rs2 = str_register_to_uint5_t(fields[2]);
            machine_code[i].s_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "blt") == 0 && num_fields == 4)
        {
            machine_code[i].s_type.opcode = BRANCH_OPCODE;
            machine_code[i].s_type.funct3 = BLT_FUNCT3;
            machine_code[i].s_type.rs1 = str_register_to_uint5_t(fields[1]);
            machine_code[i].s_type.rs2 = str_register_to_uint5_t(fields[2]);
            machine_code[i].s_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "bltu") == 0 && num_fields == 4)
        {
            machine_code[i].s_type.opcode = BRANCH_OPCODE;
            machine_code[i].s_type.funct3 = BLTU_FUNCT3;
            machine_code[i].s_type.rs1 = str_register_to_uint5_t(fields[1]);
            machine_code[i].s_type.rs2 = str_register_to_uint5_t(fields[2]);
            machine_code[i].s_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "bge") == 0 && num_fields == 4)
        {
            machine_code[i].s_type.opcode = BRANCH_OPCODE;
            machine_code[i].s_type.funct3 = BGE_FUNCT3;
            machine_code[i].s_type.rs1 = str_register_to_uint5_t(fields[1]);
            machine_code[i].s_type.rs2 = str_register_to_uint5_t(fields[2]);
            machine_code[i].s_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "bgeu") == 0 && num_fields == 4)
        {
            machine_code[i].s_type.opcode = BRANCH_OPCODE;
            machine_code[i].s_type.funct3 = BGEU_FUNCT3;
            machine_code[i].s_type.rs1 = str_register_to_uint5_t(fields[1]);
            machine_code[i].s_type.rs2 = str_register_to_uint5_t(fields[2]);
            machine_code[i].s_type.imm = str_imm_to_intn_t(fields[3], 12);
        }

        // Load and store
        else if (strcmp(operation, "lw") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = LOAD_OPCODE;
            machine_code[i].i_type.funct3 = LW_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "lh") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = LOAD_OPCODE;
            machine_code[i].i_type.funct3 = LH_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "lhu") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = LOAD_OPCODE;
            machine_code[i].i_type.funct3 = LHU_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "lb") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = LOAD_OPCODE;
            machine_code[i].i_type.funct3 = LB_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "lbu") == 0 && num_fields == 4)
        {
            machine_code[i].i_type.opcode = LOAD_OPCODE;
            machine_code[i].i_type.funct3 = LBU_FUNCT3;
            machine_code[i].i_type.rd = str_register_to_uint5_t(fields[1]);
            machine_code[i].i_type.rs1 = str_register_to_uint5_t(fields[2]);
            machine_code[i].i_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "sw") == 0 && num_fields == 4)
        {
            machine_code[i].s_type.opcode = STORE_OPCODE;
            machine_code[i].s_type.funct3 = SW_FUNCT3;
            machine_code[i].s_type.rs1 = str_register_to_uint5_t(fields[1]);
            machine_code[i].s_type.rs2 = str_register_to_uint5_t(fields[2]);
            machine_code[i].s_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "sh") == 0 && num_fields == 4)
        {
            machine_code[i].s_type.opcode = STORE_OPCODE;
            machine_code[i].s_type.funct3 = SH_FUNCT3;
            machine_code[i].s_type.rs1 = str_register_to_uint5_t(fields[1]);
            machine_code[i].s_type.rs2 = str_register_to_uint5_t(fields[2]);
            machine_code[i].s_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else if (strcmp(operation, "sb") == 0 && num_fields == 4)
        {
            machine_code[i].s_type.opcode = STORE_OPCODE;
            machine_code[i].s_type.funct3 = SB_FUNCT3;
            machine_code[i].s_type.rs1 = str_register_to_uint5_t(fields[1]);
            machine_code[i].s_type.rs2 = str_register_to_uint5_t(fields[2]);
            machine_code[i].s_type.imm = str_imm_to_intn_t(fields[3], 12);
        }
        else
        {
            perror("Error: Invalid operation or number of fields.\n");
            exit(EXIT_FAILURE);
        }

        current = current->next;
        i++;
    }
}

void write_machine_code_to_file(char *filename, rv32i_instruction *machine_code, uint32_t num_instructions)
{
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        perror("Error: Could not open file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < num_instructions; i++)
    {
        fwrite(&machine_code[i], sizeof(rv32i_instruction), 1, fp);
    }

    fclose(fp);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    node *head = asm_to_lines_list(argv[1]);
    lines_list_to_word_lists(head);

    uint32_t num_instructions = get_num_instructions(head);
    rv32i_instruction machine_code[num_instructions];
    word_lists_to_machine_code(head, machine_code);
    write_machine_code_to_file(argv[2], machine_code, num_instructions);

    return 0;
}
