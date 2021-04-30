/* Includes ------------------------------------------------------------------*/
#include "brainfuck.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* Definitions ---------------------------------------------------------------*/

/* Static functions ----------------------------------------------------------*/
static void print_error(const char *msg, int error_code)
{
        fprintf(stderr, "%s -> %s\n", msg, strerror(error_code));
}

static int interpret_input(char input, struct brainfuck_instruction *output)
{
        switch (input)
        {
        case '>':
                brainfuck_instruction_set_opcode(
                                output, BRAINFUCK_INSTRUCTION_OPCODE_MOVE);
                brainfuck_instruction_set_value(output, 1);
                break;

        case '<':
                brainfuck_instruction_set_opcode(
                                output, BRAINFUCK_INSTRUCTION_OPCODE_MOVE);
                brainfuck_instruction_set_value(output, -1);
                break;

        case '+':
                brainfuck_instruction_set_opcode(
                                output, BRAINFUCK_INSTRUCTION_OPCODE_MODIFY);
                brainfuck_instruction_set_value(output, 1);
                break;

        case '-':
                brainfuck_instruction_set_opcode(
                                output, BRAINFUCK_INSTRUCTION_OPCODE_MODIFY);
                brainfuck_instruction_set_value(output, -1);
                break;

        case ',':
                brainfuck_instruction_set_opcode(
                                output, BRAINFUCK_INSTRUCTION_OPCODE_READ);
                break;

        case '.':
                brainfuck_instruction_set_opcode(
                                output, BRAINFUCK_INSTRUCTION_OPCODE_WRITE);
                break;

        case '[':
                brainfuck_instruction_set_opcode(
                                output, BRAINFUCK_INSTRUCTION_OPCODE_JUMP_0);
                break;

        case ']':
                brainfuck_instruction_set_opcode(
                        output, BRAINFUCK_INSTRUCTION_OPCODE_JUMP_NON_0);
                break;
        
        default:
                return -1;
                break;
        }

        return 0;
}

static int optimize_output(struct brainfuck_instruction_list *list)
{
        struct brainfuck_instruction *instruction;
        enum brainfuck_instruction_opcode opcode;
        unsigned int value;
        struct brainfuck_instruction *previous_instruction;
        enum brainfuck_instruction_opcode previous_opcode;
        unsigned int previous_value;
        unsigned int i;
        int res;

        instruction = brainfuck_instruction_list_get(list, 0);
        if (!instruction)
                return 0;

        previous_opcode = BRAINFUCK_INSTRUCTION_OPCODE_ERROR;
        i = 0;

        while (i < brainfuck_instruction_list_get_size(list)) {
                instruction = brainfuck_instruction_list_get(list, i);
                opcode = brainfuck_instruction_get_opcode(instruction);
                value = brainfuck_instruction_get_value(instruction);

                if (opcode != previous_opcode ||
                    opcode != BRAINFUCK_INSTRUCTION_OPCODE_MOVE &&
                    opcode != BRAINFUCK_INSTRUCTION_OPCODE_MODIFY) {

                        previous_instruction = instruction;
                        previous_opcode = opcode;
                        previous_value = value;
                        i++;
                        continue;
                }

                previous_value += value;
                brainfuck_instruction_set_value(
                                previous_instruction, previous_value);
                res = brainfuck_instruction_list_remove(list, i);
                if (res < 0)
                        return res;
        }

        return 0;
}

static int resolve_jumps(struct brainfuck_instruction_list *list)
{
        struct brainfuck_instruction *anchor;
        struct brainfuck_instruction *instruction;
        enum brainfuck_instruction_opcode opcode;
        unsigned int i;
        unsigned int cur;
        unsigned int count;
        const size_t size = brainfuck_instruction_list_get_size(list);

        for (i = 0; i < size; i++) {
                anchor = brainfuck_instruction_list_get(list, i);
                opcode = brainfuck_instruction_get_opcode(anchor);

                if (opcode != BRAINFUCK_INSTRUCTION_OPCODE_JUMP_0)
                        continue;

                count = 1;
                cur = i;
                while (count > 0) {
                        cur++;
                        instruction = brainfuck_instruction_list_get(list, cur);
                        if (!instruction)
                                return -ENODATA;

                        opcode = brainfuck_instruction_get_opcode(instruction);
                        if (opcode == BRAINFUCK_INSTRUCTION_OPCODE_JUMP_NON_0)
                                count--;
                        else if (opcode == BRAINFUCK_INSTRUCTION_OPCODE_JUMP_0)
                                count++;
                }

                brainfuck_instruction_set_value(anchor, cur);
                brainfuck_instruction_set_value(instruction, i);
        }
}

/* Main ----------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
        struct brainfuck_instruction_list *list;
        struct brainfuck_instruction *instruction;
        char input;
        ssize_t read_size;
        int res;
        int status = EXIT_FAILURE;

        list = brainfuck_instruction_list_create();
        if (!list) {
                print_error("Could not create list", ENOMEM);
                goto error_create;
        }

        while (read_size = read(STDIN_FILENO, &input, sizeof(input)),
               read_size > 0) {
                
                res = interpret_input(input, instruction);
                if (res < 0)
                        continue;

                res = brainfuck_instruction_list_add(list, instruction);
                if (res < 0) {
                        print_error("Could not add instruction", -res);
                        goto error_add;
                }
        }

        res = optimize_output(list);
        if (res < 0) {
                print_error("Could not optimize output", -res);
                goto error_optimize;
        }

        res = resolve_jumps(list);
        if (res < 0) {
                print_error("Could not resolve jumps", -res);
                goto error_jumps;
        }

        res = brainfuck_instruction_list_write(list, STDOUT_FILENO);
        if (res < 0) {
                print_error("Could not write list", -res);
                goto error_write;
        }
        
        status = EXIT_SUCCESS;
error_write:
error_jumps:
error_optimize:
error_add:
        brainfuck_instruction_list_destroy(list);
error_create:
        return status;
}
