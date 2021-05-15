#ifndef BRAINFUCK_H
#define BRAINFUCK_H

/* Includes ------------------------------------------------------------------*/
#include <sys/types.h>

/* Definitions ---------------------------------------------------------------*/
struct brainfuck_instruction;

struct brainfuck_instruction_list;

enum brainfuck_instruction_opcode {
        BRAINFUCK_INSTRUCTION_OPCODE_ERROR = -1,
        BRAINFUCK_INSTRUCTION_OPCODE_END,
        BRAINFUCK_INSTRUCTION_OPCODE_MOVE,
        BRAINFUCK_INSTRUCTION_OPCODE_MODIFY,
        BRAINFUCK_INSTRUCTION_OPCODE_READ,
        BRAINFUCK_INSTRUCTION_OPCODE_WRITE,
        BRAINFUCK_INSTRUCTION_OPCODE_JUMP_0,
        BRAINFUCK_INSTRUCTION_OPCODE_JUMP_NON_0,
        BRAINFUCK_INSTRUCTION_OPCODE_RESET
};

/* Functions -----------------------------------------------------------------*/
struct brainfuck_instruction_list *brainfuck_instruction_list_create();

void brainfuck_instruction_list_destroy(
                struct brainfuck_instruction_list *list);

int brainfuck_instruction_list_read(
                struct brainfuck_instruction_list *list, int fd);

int brainfuck_instruction_list_write(
                const struct brainfuck_instruction_list *list, int fd);

ssize_t brainfuck_instruction_list_get_size(
                const struct brainfuck_instruction_list *list);

int brainfuck_instruction_list_add(
                struct brainfuck_instruction_list *list,
                const struct brainfuck_instruction *instruction);

int brainfuck_instruction_list_remove(
                struct brainfuck_instruction_list *list, unsigned int index);

struct brainfuck_instruction *brainfuck_instruction_list_get(
                const struct brainfuck_instruction_list *list,
                unsigned int index);

enum brainfuck_instruction_opcode brainfuck_instruction_get_opcode(
                const struct brainfuck_instruction *instruction);

int brainfuck_instruction_set_opcode(
                struct brainfuck_instruction *instruction,
                enum brainfuck_instruction_opcode opcode);

int brainfuck_instruction_get_value(
                const struct brainfuck_instruction *instruction);

int brainfuck_instruction_set_value(
                struct brainfuck_instruction *instruction, unsigned int value);

#endif /* BRAINFUCK_H */
