#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include "stack.h"

#define TAPE_LENGTH 30000 
#ifdef DEBUG
    #define dprintf(...) printf(__VA_ARGS__)
	#define dshow_stack(...) show_stack(__VA_ARGS__)
#else
    #define dprintf(...) ((void)0)
	#define dshow_stack(...) show_stack(__VA_ARGS__)
#endif
#define wprintf(...); if (!mute_warnings){ printf(__VA_ARGS__); }

void *mem_strt;
FILE *program_file;
STACK while_stack;
bool mute_warnings = true;
const char* HELP_MSG =
	"Usage: brainfuck [-m] [OPTIONS] <FILE>\n"
	"\n"
	"Interpreter for Brainfuck programs.\n"
	"\n"
	"Options:\n"
	"  -m                 Unmute warnings (enable debug output)\n"
	"  -l <TAPE LENGTH>   Set the size of the memory tape (default: 30000)\n"
	"\n"
	"Notes:\n"
	"  • -m must be provided before other options\n"
	"  • The order of options does not matter otherwise, but -l must be followed by a valid integer.\n"
	"  • The FILE argument is required and should point to a valid Brainfuck source file.\n";

void parse_brainfuck(FILE *file, void* mem_tape, int tape_length);
void cleanup();
void handle_signal(int sig);

int main(int argc, char const *argv[])
{
	if (argc < 2 || argc > 5){
		puts(HELP_MSG);
		return 0;
	}

	int i = 1;
	char* file_name = NULL;
	int mem_length = -1;

	while (i < argc){
		if (strcmp(argv[i], "-m") == 0){
			mute_warnings = false;
		}
		else if (strcmp(argv[i],"-l") == 0){
			if (argc > i + 1){
				char *length_str = argv[i+1];
				char *end;
				mem_length = (int) strtol(length_str, &end, 10);

				if (*end != '\0' || mem_length <= 0) {
				    wprintf("[WARNING]: -l tag provided but TAPE LENGTH=\"%s\" was invalid specified. Defaulting to TAPE LENGTH=%d.\n", length_str, TAPE_LENGTH);
				    mem_length = TAPE_LENGTH;
				}
			} else {
				mem_length = TAPE_LENGTH;
				wprintf("[WARNING]: -l tag provided but TAPE LENGTH not specified. Defaulting to TAPE LENGTH=%d.\n", TAPE_LENGTH);
			}

			if (file_name != NULL){ //already found file name
				break;
			} else { //keep looking
				i++;
			}
		} else { //defining file name
			if (file_name != NULL){
				wprintf("[WARNING]: More than one program file specified. Forgetting last file name \"%s\" and choosing \"%s\".\n", file_name, argv[i]);
			}
			file_name = argv[i];
		}

		i++;
	}
	if (mem_length < 0){
		//mem_length has not been set
		mem_length = TAPE_LENGTH;
	}


	dprintf("[DEBUG]: File Name = \"%s\" Memory Length = %d\n",file_name, mem_length);
	
	signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

	program_file = fopen(file_name, "r");
	if (program_file == NULL){
		printf("[FAULT]: Error opening file \"%s\".\n", file_name);
		exit(1);
	}

	mem_strt = calloc(mem_length, sizeof(char));
	if (mem_strt == NULL){
		printf("[FAULT]: Could not allocate memory tape of size %d\n", mem_length);
		fclose(program_file);
		exit(1);
	}

	parse_brainfuck(program_file, mem_strt, mem_length);

	fclose(program_file);
	free(mem_strt);
	return 0;
}

void cleanup(){
	if (program_file != NULL){
		fclose(program_file);
	}
	if (mem_strt != NULL){
		free(mem_strt);
	}
	if (while_stack != NULL){
		cleanup_stack(while_stack);
	}
}

void handle_signal(int sig){
	printf("[EXIT]: Caught signal %d, cleaning up...\n", sig);
    cleanup();
    exit(1);
}

void parse_brainfuck(FILE *file, void *mem_strt, int tape_length){
	while_stack = create_stack();
	
	char *mem_tape = (char *)mem_strt;
	int ptr = 0;
	size_t cmd_line = 1;
	size_t cmd_pos = 1;
	size_t cmd_pos_save;

	char cmd;
	while ((cmd = fgetc(file)) != EOF){

		dprintf("[DEBUG]: Executing '%c' at [%u,%u].\n", cmd, cmd_line, cmd_pos);

		switch(cmd){
		case '>':
			if (ptr + 1 >= tape_length){
				printf("[FAULT]: Memory out of bounds move at [%u,%u].\n", cmd_line, cmd_pos);

				cleanup_stack(while_stack);
				return;
			}
			ptr++;
			break;
		case '<':
			if (ptr - 1 < 0){
				printf("[FAULT]: Memory out of bounds move at [%u,%u].\n", cmd_line, cmd_pos);

				cleanup_stack(while_stack);
				return;
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
				wprintf("[WARNING]: Memory overflow at [%u,%u].\n", cmd_line, cmd_pos);
				mem_tape[ptr] = 255;
			} else {
				mem_tape[ptr]--;
			}
			break;
		case ',':
			mem_tape[ptr] = getchar();
			break;
		case '.':
			putchar(mem_tape[ptr]);
			break;
		case '[':
			stack_push(while_stack, ftell(file));
			cmd_pos_save = cmd_pos;
			break;
		case ']':
			if (is_stack_empty(while_stack)){
				printf("[FAULT]: Illegal stack return at [%u,%u].\n", cmd_line, cmd_pos);

				cleanup_stack(while_stack);
				return;
			}

			size_t seek_loc;
			if (!stack_pop(while_stack, &seek_loc)){
				printf("[FAULT]: Error while finding While start at [%u,%u].\n", cmd_line, cmd_pos);

				cleanup_stack(while_stack);
				return;
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

			cleanup_stack(while_stack);
			return;
		}
		cmd_pos++;
	}

	cleanup_stack(while_stack);
}
