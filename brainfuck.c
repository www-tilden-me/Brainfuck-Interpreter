#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//The brainfuck memory tape is supposed to be 30,000 bytes?
#define TAPE_LENGTH 30000 
#define DEBUG true
const char* HELP_MSG = "brainfuck [FILE]\n"\
					"brainfuck [FILE] -l [TAPE LENGTH]";

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

	printf("FileName = %s\nMemLength = %d\n",file_name, mem_length);
	
	exit(0);
	return 0;
}


