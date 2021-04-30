/* Includes ------------------------------------------------------------------*/
#include "brainfuck.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* Definitions ---------------------------------------------------------------*/

#define MEMORY_SIZE 30000

/* Static functions ----------------------------------------------------------*/
static void print_error(const char *msg, int error_code)
{
        fprintf(stderr, "%s -> %s\n", msg, strerror(error_code));
}

static void run_list(
		const struct brainfuck_instruction_list *list,
		const char *input)
{
	struct brainfuck_instruction *instruction;
	enum brainfuck_instruction_opcode opcode;
	unsigned int value;
	const size_t size = brainfuck_instruction_list_get_size(list);
	unsigned int i;
	char buf[MEMORY_SIZE];
	unsigned int cursor = MEMORY_SIZE / 2;
	const char *input_cursor = input;

	memset(buf, 0, sizeof(buf));

	for (i = 0; i < size; i++) {
		instruction = brainfuck_instruction_list_get(list, i);
		opcode = brainfuck_instruction_get_opcode(instruction);
		value = brainfuck_instruction_get_value(instruction);

		switch (opcode)
		{
		case BRAINFUCK_INSTRUCTION_OPCODE_MOVE:
			cursor += value;
			break;

		case BRAINFUCK_INSTRUCTION_OPCODE_MODIFY:
			buf[cursor] += value;
			break;

		case BRAINFUCK_INSTRUCTION_OPCODE_READ:
			buf[cursor] = *input_cursor;
			input_cursor++;
			break;

		case BRAINFUCK_INSTRUCTION_OPCODE_WRITE:
			putc(buf[cursor], stdout);
			break;

		case BRAINFUCK_INSTRUCTION_OPCODE_JUMP_0:
			if (!buf[cursor])
				i = value;
			break;

		case BRAINFUCK_INSTRUCTION_OPCODE_JUMP_NON_0:
			if (buf[cursor])
				i = value;
			break;
		
		default:
			break;
		}
	}
}

/* Main ----------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
        struct brainfuck_instruction_list *list;
        int res;
        int status = EXIT_FAILURE;

        list = brainfuck_instruction_list_create();
	if (!list) {
		print_error("Could not create list", ENOMEM);
		goto error_create;
	}

	res = brainfuck_instruction_list_read(list, STDIN_FILENO);
	if (res < 0) {
		print_error("Could not read list", -res);
		goto error_read;
	}

	run_list(list, "Hello world!");
        status = EXIT_SUCCESS;
error_run:
error_read:
        brainfuck_instruction_list_destroy(list);
error_create:
        return status;
}
