#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define TAPE_LENGTH 30000 
#define DEBUG true

const char* HELP_MSG = "brainfuck [FILE]\n"\
					"brainfuck [FILE] -l [TAPE LENGTH]";

void parse_brainfuck(FILE *file, void* mem_tape, int tape_length);

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
				    printf("[WARNING]: -l tag provided but TAPE LENGTH=\"%s\" was invalid specified. Defaulting to TAPE LENGTH=%d.\n", length_str, TAPE_LENGTH);
				    mem_length = TAPE_LENGTH;
				}
			} else {
				mem_length = TAPE_LENGTH;
				printf("[WARNING]: -l tag provided but TAPE LENGTH not specified. Defaulting to TAPE LENGTH=%d.\n", TAPE_LENGTH);
			}

			if (file_name != NULL){ //already found file name
				break;
			} else { //keep looking
				i++;
			}
		} else { //defining file name
			if (file_name != NULL){
				printf("[WARNING]: More than one program file specified. Forgetting last file name \"%s\" and choosing \"%s\".\n", file_name, argv[i]);
			}
			file_name = argv[i];
		}

		i++;
	}
	if (mem_length < 0){
		//mem_length has not been set
		mem_length = TAPE_LENGTH;
	}

	if (DEBUG){
		printf("[DEBUG]: File Name = \"%s\" MemLength = %d\n",file_name, mem_length);
	}
	
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

void parse_brainfuck(FILE *file, void *mem_tape, int tape_length){
	printf("IN HERE!\n");
}

