/* Includes ------------------------------------------------------------------*/
#include "brainfuck.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Definitions ---------------------------------------------------------------*/
#define DEFAULT_LIST_RESERVE 10

struct brainfuck_instruction {
        enum brainfuck_instruction_opcode opcode;
        unsigned int value;
};

struct brainfuck_instruction_list {
        struct brainfuck_instruction *list;
        size_t size;
        size_t reserve;
};

/* Static functions ----------------------------------------------------------*/
static int resize_list(struct brainfuck_instruction_list *list)
{
        struct brainfuck_instruction *new_list;
        unsigned int new_reserve;

        if (list->reserve == 0)
                new_reserve = DEFAULT_LIST_RESERVE;
        else
                new_reserve = list->reserve * 2;

        new_list = realloc(list->list, sizeof(*list->list) * new_reserve);
        if (!new_list)
                return -ENOMEM;

        list->list = new_list;
        list->reserve = new_reserve;
        return 0;
}


/* Functions -----------------------------------------------------------------*/
struct brainfuck_instruction_list *brainfuck_instruction_list_create()
{
        struct brainfuck_instruction_list *list;

        list = malloc(sizeof(*list));
        if (!list)
                return NULL;

        list->list = NULL;
        list->size = 0;
        list->reserve = 0;

        return list;
}

void brainfuck_instruction_list_destroy(struct brainfuck_instruction_list *list)
{
        if (!list)
                return;

        free(list->list);
        list->list = NULL;
        list->size = 0;
        list->reserve = 0;
}

int brainfuck_instruction_list_read(
                struct brainfuck_instruction_list *list, int fd)
{
        ssize_t read_size;
        struct brainfuck_instruction instruction;
        int res;

        if (!list)
                return -EINVAL;

        while (1) {
                read_size = read(fd, &instruction, sizeof(instruction));
                if (read_size < 0)
                        return -errno;
                else if (read_size == 0)
                        break;

                res = brainfuck_instruction_list_add(list, &instruction);
                if (res < 0)
                        return res;
        }

        return 0;
}

int brainfuck_instruction_list_write(
                const struct brainfuck_instruction_list *list, int fd)
{
        ssize_t write_size;
        size_t expected_write_size;
        unsigned int i;

        if (!list)
                return -EINVAL;

        expected_write_size = list->size * sizeof(*list->list);
        write_size = write(fd, list->list, expected_write_size);
        if (write_size < expected_write_size)
                return -EIO;

        return 0;
}

int brainfuck_instruction_list_add(
                struct brainfuck_instruction_list *list,
                const struct brainfuck_instruction *instruction)
{
        int res;

        if (!list || !instruction)
                return -EINVAL;

        if (list->size == list->reserve) {
                res = resize_list(list);
                if (res < 0)
                        goto error_resize;
        }

        list->list[list->size] = *instruction;
        list->size++;
        res = 0;

error_resize:
        return res;
}

ssize_t brainfuck_instruction_list_get_size(
                const struct brainfuck_instruction_list *list)
{
        if (!list)
                return -EINVAL;

        return list->size;
}

int brainfuck_instruction_list_remove(
                struct brainfuck_instruction_list *list, unsigned int index)
{
        if (!list || index >= list->size)
                return -EINVAL;

        if (index < list->size - 1) {
                memcpy(&list->list[index], &list->list[index + 1],
                        (list->size - index - 1) * sizeof(*list->list));
        }

        list->size--;
        return 0;
}

struct brainfuck_instruction *brainfuck_instruction_list_get(
                const struct brainfuck_instruction_list *list,
                unsigned int index)
{
        if (!list || index >= list->size)
                return NULL;

        return &list->list[index];
}

enum brainfuck_instruction_opcode brainfuck_instruction_get_opcode(
                const struct brainfuck_instruction *instruction)
{
        if (!instruction)
                return BRAINFUCK_INSTRUCTION_OPCODE_ERROR;

        return instruction->opcode;
}

int brainfuck_instruction_set_opcode(
                struct brainfuck_instruction *instruction,
                enum brainfuck_instruction_opcode opcode)
{
        if (!instruction || opcode == BRAINFUCK_INSTRUCTION_OPCODE_ERROR)
                return -EINVAL;

        instruction->opcode = opcode;
        return 0;
}

int brainfuck_instruction_get_value(
                const struct brainfuck_instruction *instruction)
{
        if (!instruction)
                return -EINVAL;

        return instruction->value;

}

int brainfuck_instruction_set_value(
                struct brainfuck_instruction *instruction, unsigned int value)
{
        if (!instruction)
                return -EINVAL;

        instruction->value = value;
        return 0;
}
