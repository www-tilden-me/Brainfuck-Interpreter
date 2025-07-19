#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define TAPE_LENGTH 30000 
#ifdef DEBUG
    #define dprintf(...) printf(__VA_ARGS__)
#else
    #define dprintf(...) ((void)0)
#endif

const char* HELP_MSG = "brainfuck [FILE]\n"\
					"brainfuck [FILE] -l [TAPE LENGTH]";

void parse_brainfuck(FILE *file, void* mem_tape, int tape_length);

// TODO: Handle SIG
int main(int argc, char const *argv[])
{
	if (argc < 2 || argc > 4){
		puts(HELP_MSG);
		return 0;
	}

	int i = 1;
	char* file_name = NULL;
	int mem_length = -1;
	while (i < argc){
		if (strcmp(argv[i],"-l") == 0){
			if (argc > i + 1){
				char *length_str = argv[i+1];
				char *end;
				mem_length = (int) strtol(length_str, &end, 10);

				if (*end != '\0' || mem_length <= 0) {
				    dprintf("[WARNING]: -l tag provided but TAPE LENGTH=\"%s\" was invalid specified. Defaulting to TAPE LENGTH=%d.\n", length_str, TAPE_LENGTH);
				    mem_length = TAPE_LENGTH;
				}
			} else {
				mem_length = TAPE_LENGTH;
				dprintf("[WARNING]: -l tag provided but TAPE LENGTH not specified. Defaulting to TAPE LENGTH=%d.\n", TAPE_LENGTH);
			}

			if (file_name != NULL){ //already found file name
				break;
			} else { //keep looking
				i++;
			}
		} else { //defining file name
			if (file_name != NULL){
				dprintf("[WARNING]: More than one program file specified. Forgetting last file name \"%s\" and choosing \"%s\".\n", file_name, argv[i]);
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
	
	FILE *file = fopen(file_name, "r");
	if (file == NULL){
		printf("[FAULT]: Error opening file \"%s\".\n", file_name);
		exit(1);
	}

	void* mem_strt = calloc(mem_length, sizeof(char));
	if (mem_strt == NULL){
		printf("[FAULT]: Could not allocate memory tape of size %d\n", mem_length);
		fclose(file);
		exit(1);
	}

	parse_brainfuck(file, mem_strt, mem_length);

	fclose(file);
	free(mem_strt);
	return 0;
}

void parse_brainfuck(FILE *file, void *mem_strt, int tape_length){
	char *mem_tape = (char *)mem_strt;
	int ptr = 0;
	size_t cmd_line = 1;
	size_t cmd_pos = 1;
	size_t cmd_pos_save;

	size_t while_start;

	char cmd;
	while ((cmd = fgetc(file)) != EOF){
		switch(cmd){
		case '>':
			if (ptr + 1 >= tape_length){
				printf("[FAULT]: Memory out of bounds move at [%u,%u].\n", cmd_line, cmd_pos);
				return;
			}
			ptr++;
			break;
		case '<':
			if (ptr - 1 < 0){
				printf("[FAULT]: Memory out of bounds move at [%u,%u].\n", cmd_line, cmd_pos);
				return;
			}
			ptr--;
			break;
		case '+':
			if ((unsigned char)mem_tape[ptr] == 255){
				dprintf("[WARNING]: Memory overflow at [%u,%u]\n", cmd_line, cmd_pos);
				mem_tape[ptr] = 0;
			} else {
				mem_tape[ptr]++;
			}
			break;
		case '-':
			if ((unsigned char)mem_tape[ptr] == 0){
				dprintf("[WARNING]: Memory overflow at [%u,%u]\n", cmd_line, cmd_pos);
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
			while_start = ftell(file); //TODO: Stack for multiple whiles chained
			cmd_pos_save = cmd_pos;
			break;
		case ']':
			if ((unsigned char)mem_tape[ptr] != 0){
				fseek(file, while_start, SEEK_SET);
				cmd_pos = cmd_pos_save;
			}
			break;

		case '\n':
			cmd_line++;
			cmd_pos = 0;
			break;
		case ' ':
		case '\t':
			break;
		default:
			printf("[FAULT]: Illegal command \"%c\" at [%u,%u]\n",cmd, cmd_line, cmd_pos);
			return;
		}
		cmd_pos++;
	}
}

