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

uint32_t get_num_insts(node *head)
{
    node *current = head;
    uint32_t num_insts = 0;

    while (current != NULL)
    {
        num_insts++;
        current = current->next;
    }

    return num_insts;
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

void append_i_type(rv32i_t *inst, uint8_t opcode, char **fields)
{
    inst->i_type.opcode = opcode;
    inst->i_type.rd = str_register_to_uint5_t(fields[1]);
    inst->i_type.rs1 = str_register_to_uint5_t(fields[2]);
    inst->i_type.imm = str_imm_to_intn_t(fields[3], 12);
}

void append_i_type_shift(rv32i_t *inst, uint8_t opcode, char **fields)
{
    inst->i_type_shift.opcode = opcode;
    inst->i_type_shift.rd = str_register_to_uint5_t(fields[1]);
    inst->i_type_shift.rs1 = str_register_to_uint5_t(fields[2]);
    inst->i_type_shift.shamt = str_imm_to_uintn_t(fields[3], 5);
}

void append_u_type(rv32i_t *inst, uint8_t opcode, char **fields)
{
    inst->u_type.opcode = opcode;
    inst->u_type.rd = str_register_to_uint5_t(fields[1]);
    inst->u_type.imm = str_imm_to_intn_t(fields[2], 20);
}

void append_r_type(rv32i_t *inst, uint8_t opcode, char **fields)
{
    inst->r_type.opcode = opcode;
    inst->r_type.rd = str_register_to_uint5_t(fields[1]);
    inst->r_type.rs1 = str_register_to_uint5_t(fields[2]);
    inst->r_type.rs2 = str_register_to_uint5_t(fields[3]);
}

void append_s_type(rv32i_t *inst, uint8_t opcode, char **fields)
{
    inst->s_type.opcode = opcode;
    inst->s_type.rs1 = str_register_to_uint5_t(fields[1]);
    inst->s_type.rs2 = str_register_to_uint5_t(fields[2]);
    inst->s_type.imm = str_imm_to_intn_t(fields[3], 12);
}

void word_lists_to_machine_code(node *head, rv32i_t *machine_code)
{
    node *current = head;
    uint32_t i = 0;

    while (current != NULL)
    {
        rv32i_t *inst = &machine_code[i];
        char **fields = current->words;
        char *opcode = fields[0];
        uint8_t num_fields = current->num_words;

        if (strcmp(opcode, "addi") == 0 && num_fields == 4)
        {
            append_i_type(inst, ADDI, fields);
        }
        else if (strcmp(opcode, "slti") == 0 && num_fields == 4)
        {
            append_i_type(inst, SLTI, fields);
        }
        else if (strcmp(opcode, "sltiu") == 0 && num_fields == 4)
        {
            inst->i_type.opcode = SLTIU;
            inst->i_type.rd = str_register_to_uint5_t(fields[1]);
            inst->i_type.rs1 = str_register_to_uint5_t(fields[2]);
            uint32_t imm = (uint32_t)str_to_int32_t(fields[3]);
            if (imm > 0x7FF && imm < 0xfffff800)
            {
                perror("Error: Immediate out of range\n");
                exit(EXIT_FAILURE);
            }
            inst->i_type.imm = imm;
        }
        else if (strcmp(opcode, "andi") == 0 && num_fields == 4)
        {
            append_i_type(inst, ANDI, fields);
        }
        else if (strcmp(opcode, "ori") == 0 && num_fields == 4)
        {
            append_i_type(inst, ORI, fields);
        }
        else if (strcmp(opcode, "xori") == 0 && num_fields == 4)
        {
            append_i_type(inst, XORI, fields);
        }
        else if (strcmp(opcode, "slli") == 0 && num_fields == 4)
        {
            append_i_type_shift(inst, SLLI, fields);
        }
        else if (strcmp(opcode, "srli") == 0 && num_fields == 4)
        {
            append_i_type_shift(inst, SRLI, fields);
        }
        else if (strcmp(opcode, "srai") == 0 && num_fields == 4)
        {
            append_i_type_shift(inst, SRAI, fields);
        }
        else if (strcmp(opcode, "lui") == 0 && num_fields == 3)
        {
            append_u_type(inst, LUI, fields);
        }
        else if (strcmp(opcode, "auipc") == 0 && num_fields == 3)
        {
            append_u_type(inst, AUIPC, fields);
        }
        else if (strcmp(opcode, "add") == 0 && num_fields == 4)
        {
            append_r_type(inst, ADD, fields);
        }
        else if (strcmp(opcode, "slt") == 0 && num_fields == 4)
        {
            append_r_type(inst, SLT, fields);
        }
        else if (strcmp(opcode, "sltu") == 0 && num_fields == 4)
        {
            append_r_type(inst, SLTU, fields);
        }
        else if (strcmp(opcode, "and") == 0 && num_fields == 4)
        {
            append_r_type(inst, AND, fields);
        }
        else if (strcmp(opcode, "or") == 0 && num_fields == 4)
        {
            append_r_type(inst, OR, fields);
        }
        else if (strcmp(opcode, "xor") == 0 && num_fields == 4)
        {
            append_r_type(inst, XOR, fields);
        }
        else if (strcmp(opcode, "sll") == 0 && num_fields == 4)
        {
            append_r_type(inst, SLL, fields);
        }
        else if (strcmp(opcode, "srl") == 0 && num_fields == 4)
        {
            append_r_type(inst, SRL, fields);
        }
        else if (strcmp(opcode, "sub") == 0 && num_fields == 4)
        {
            append_r_type(inst, SUB, fields);
        }
        else if (strcmp(opcode, "sra") == 0 && num_fields == 4)
        {
            append_r_type(inst, SRA, fields);
        }
        else if (strcmp(opcode, "jal") == 0 && num_fields == 3)
        {
            append_u_type(inst, JAL, fields);
        }
        else if (strcmp(opcode, "jalr") == 0 && num_fields == 4)
        {
            append_i_type(inst, JALR, fields);
        }
        else if (strcmp(opcode, "beq") == 0 && num_fields == 4)
        {
            append_s_type(inst, BEQ, fields);
        }
        else if (strcmp(opcode, "bne") == 0 && num_fields == 4)
        {
            append_s_type(inst, BNE, fields);
        }
        else if (strcmp(opcode, "blt") == 0 && num_fields == 4)
        {
            append_s_type(inst, BLT, fields);
        }
        else if (strcmp(opcode, "bltu") == 0 && num_fields == 4)
        {
            append_s_type(inst, BLTU, fields);
        }
        else if (strcmp(opcode, "bge") == 0 && num_fields == 4)
        {
            append_s_type(inst, BGE, fields);
        }
        else if (strcmp(opcode, "bgeu") == 0 && num_fields == 4)
        {
            append_s_type(inst, BGEU, fields);
        }
        else if (strcmp(opcode, "lw") == 0 && num_fields == 4)
        {
            append_i_type(inst, LW, fields);
        }
        else if (strcmp(opcode, "lh") == 0 && num_fields == 4)
        {
            append_i_type(inst, LH, fields);
        }
        else if (strcmp(opcode, "lhu") == 0 && num_fields == 4)
        {
            append_i_type(inst, LHU, fields);
        }
        else if (strcmp(opcode, "lb") == 0 && num_fields == 4)
        {
            append_i_type(inst, LB, fields);
        }
        else if (strcmp(opcode, "lbu") == 0 && num_fields == 4)
        {
            append_i_type(inst, LBU, fields);
        }
        else if (strcmp(opcode, "sw") == 0 && num_fields == 4)
        {
            append_s_type(inst, SW, fields);
        }
        else if (strcmp(opcode, "sh") == 0 && num_fields == 4)
        {
            append_s_type(inst, SH, fields);
        }
        else if (strcmp(opcode, "sb") == 0 && num_fields == 4)
        {
            append_s_type(inst, SB, fields);
        }
        else
        {
            perror("Error: Invalid opcode or number of fields.\n");
            exit(EXIT_FAILURE);
        }

        current = current->next;
        i++;
    }
}

void write_machine_code_to_file(char *filename, rv32i_t *machine_code, uint32_t num_insts)
{
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        perror("Error: Could not open file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < num_insts; i++)
    {
        fwrite(&machine_code[i], sizeof(rv32i_t), 1, fp);
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

    uint32_t num_insts = get_num_insts(head);
    rv32i_t machine_code[num_insts];
    word_lists_to_machine_code(head, machine_code);
    write_machine_code_to_file(argv[2], machine_code, num_insts);

    return 0;
}
