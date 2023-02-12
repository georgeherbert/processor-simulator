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

node *asm_to_lines_list(const char *filename)
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

uint32_t register_to_uint32_t(char *reg_name)
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
        printf("Error: No digits were found\n");
        exit(EXIT_FAILURE);
    }

    if (result > 31)
    {
        printf("Error: Register number too large\n");
        exit(EXIT_FAILURE);
    }

    return (uint32_t)result;
}

void word_lists_to_machine_code(node *head, rv32i_instruction *machine_code)
{
    node *current = head;
    uint32_t num_instructions = 0;
    uint32_t instruction = 0;

    while (current != NULL)
    {
        if (strcmp(current->words[0], "addi") == 0 && current->num_words == 4)
        {
            machine_code[instruction].i_type.opcode = OP_IMM_OPCODE;
            printf("%u\n", register_to_uint32_t(current->words[1]));
            printf("%u\n", register_to_uint32_t(current->words[2]));
            printf("%u\n", register_to_uint32_t(current->words[3]));
        }

        current = current->next;
    }
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

    // node *current = head;
    // while (current != NULL)
    // {
    //     printf("\n%d: ", current->num_words);
    //     for (uint8_t i = 0; i < current->num_words; i++)
    //     {
    //         printf("%s ", current->words[i]);
    //     }
    //     current = current->next;
    // }

    // FILE *fp;

    // uint32_t num_words = 5;
    // rv32i_instruction machine_code[num_words];

    // machine_code[1].u_type.opcode = 12;
    // machine_code[1].u_type.rd = 3;
    // machine_code[1].u_type.imm = 4;

    // machine_code[0].u_type.imm = 100;
    // machine_code[0].u_type.rd = 5;
    // machine_code[0].u_type.opcode = 17;

    // for (uint32_t i = 0; i < num_words; i++)
    // {
    //     fwrite(&machine_code[i], sizeof(rv32i_instruction), 1, fp);
    // }

    // fclose(fp);
    return 0;
}
