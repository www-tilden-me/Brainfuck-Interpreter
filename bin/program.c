#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include "stack.h"

#ifdef DEBUG
    #define dprintf(...) printf(__VA_ARGS__)
	#define dshow_stack(...) show_stack(__VA_ARGS__)
#else
    #define dprintf(...) ((void)0)
	#define dshow_stack(...) show_stack(__VA_ARGS__)
#endif
#define wprintf(...); if (!mute_warnings){ printf(__VA_ARGS__); }
#define FAULT_CODE -1

void flush_stdin() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int parse_brainfuck(FILE *file, int ptr_start, void *mem_strt, int tape_length, 
						STACK wstack, bool mute_warnings){	

	char *mem_tape = (char *)mem_strt;
	int ptr = ptr_start;
	size_t cmd_line = 1;
	size_t cmd_pos = 1;
	size_t cmd_pos_save;

	bool wasRead = false;

	char cmd;
	while ((cmd = fgetc(file)) != EOF){

		dprintf("[DEBUG]: Executing '%c' at [%u,%u].\n", cmd, cmd_line, cmd_pos);

		switch(cmd){
		case '>':
			if (ptr + 1 >= tape_length){
				printf("[FAULT]: Memory out of bounds move at [%u,%u].\n", cmd_line, cmd_pos);
				return FAULT_CODE;
			}
			ptr++;
			break;
		case '<':
			if (ptr - 1 < 0){
				printf("[FAULT]: Memory out of bounds move at [%u,%u].\n", cmd_line, cmd_pos);
				return FAULT_CODE;
			}
			ptr--;
			break;
		case '+':
			if ((unsigned char)mem_tape[ptr] == 255){
				wprintf("[WARNING]: Memory overflow at [%u,%u].\n", cmd_line, cmd_pos);
				mem_tape[ptr] = 0;
			} else {
				mem_tape[ptr]++;
			}
			break;
		case '-':
			if ((unsigned char)mem_tape[ptr] == 0){
				wprintf("[WARNING]: Memory underflow at [%u,%u].\n", cmd_line, cmd_pos);
				mem_tape[ptr] = 255;
			} else {
				mem_tape[ptr]--;
			}
			break;
		case ',':
			mem_tape[ptr] = getchar();
			wasRead = true;
			break;
		case '.':
			putchar(mem_tape[ptr]);
			break;
		case '[':
			stack_push(wstack, ftell(file));
			cmd_pos_save = cmd_pos;
			break;
		case ']':
			if (is_stack_empty(wstack)){
				printf("[FAULT]: Illegal stack return at [%u,%u].\n", cmd_line, cmd_pos);
				return FAULT_CODE;
			}

			size_t seek_loc;
			if (!stack_pop(wstack, &seek_loc)){
				printf("[FAULT]: Error while finding While start at [%u,%u].\n", cmd_line, cmd_pos);
				return FAULT_CODE;
			}

			if ((unsigned char)mem_tape[ptr] != 0){
				fseek(file, seek_loc-1, SEEK_SET);
				cmd_pos = cmd_pos_save;
			}
			break;

		case '\n':
			cmd_line++;
			cmd_pos = 0;
			break;
		case ' ':
		case '\t':
		case '\r':
			break;

		default:
			printf("[FAULT]: Illegal command \"%c\" at [%u,%u].\n", cmd, cmd_line, cmd_pos);
			return FAULT_CODE;
		}
		cmd_pos++;
	}

	if (wasRead){ //clean extra characters in stdin -- '\n'
		flush_stdin(); //probably a better way to do this for interactive mode
	}

	return ptr;
}
