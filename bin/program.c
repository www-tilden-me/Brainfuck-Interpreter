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
	STACK cmd_pos_stack = create_stack();
	STACK cmd_line_stack = create_stack();
	char *mem_tape = (char *)mem_strt;
	int ptr = ptr_start;
	size_t curr_cmd_line = 1;
	size_t curr_cmd_pos = 1;

	bool wasRead = false;

	char cmd;
	while ((cmd = fgetc(file)) != EOF){

		dprintf("[DEBUG]: Executing '%c' at [%zu,%zu].\n", cmd, curr_cmd_line, curr_cmd_pos);

		switch(cmd){
		case '>':
			if (ptr + 1 >= tape_length){
				printf("[FAULT]: Memory out of bounds move at [%zu,%zu].\n", curr_cmd_line, curr_cmd_pos);
				return FAULT_CODE;
			}
			ptr++;
			break;
		case '<':
			if (ptr - 1 < 0){
				printf("[FAULT]: Memory out of bounds move at [%zu,%zu].\n", curr_cmd_line, curr_cmd_pos);
				return FAULT_CODE;
			}
			ptr--;
			break;
		case '+':
			if ((unsigned char)mem_tape[ptr] == 255){
				wprintf("[WARNING]: Memory overflow at [%zu,%zu].\n", curr_cmd_line, curr_cmd_pos);
				mem_tape[ptr] = 0;
			} else {
				mem_tape[ptr]++;
			}
			break;
		case '-':
			if ((unsigned char)mem_tape[ptr] == 0){
				wprintf("[WARNING]: Memory underflow at [%zu,%zu].\n", curr_cmd_line, curr_cmd_pos);
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
			if (mem_tape[ptr] != 0){
				stack_push(wstack, ftell(file));
				stack_push(cmd_line_stack, curr_cmd_line);
				stack_push(cmd_pos_stack, curr_cmd_pos);
			} else {
				int total = 1;
				char tmp;
				while ((tmp = fgetc(file)) != EOF){
					if (tmp == '['){
						total += 1;
					} else if (tmp == ']'){
						total -= 1; 
						if (total == 0){
							break;
						}
					}
				}
			}

			break;
		case ']':
			if (is_stack_empty(wstack)){
				printf("[FAULT]: Illegal stack return at [%zu,%zu].\n", curr_cmd_line, curr_cmd_pos);
				return FAULT_CODE;
			}

			size_t seek_loc;
			if (!stack_pop(wstack, &seek_loc)){
				printf("[FAULT]: Error while finding While start at [%zu,%zu].\n", curr_cmd_line, curr_cmd_pos);
				return FAULT_CODE;
			}

			if ((unsigned char)mem_tape[ptr] != 0){
				fseek(file, seek_loc-1, SEEK_SET);
				stack_pop(cmd_pos_stack, &curr_cmd_pos);
				stack_pop(cmd_line_stack, &curr_cmd_line);
				continue;
			}
			break;

		case '\n':
			curr_cmd_line++;
			curr_cmd_pos = 0;
			break;
		case ' ':
		case '\t':
		case '\r':
			break;

		default:
			printf("[FAULT]: Illegal command \"%c\" at [%zu,%zu].\n", cmd, curr_cmd_line, curr_cmd_pos);
			return FAULT_CODE;
		}
		curr_cmd_pos++;
	}

	if (wasRead){ //clean extra characters in stdin -- '\n'
		flush_stdin(); //probably a better way to do this for interactive mode
	}

	return ptr;
}
