#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include "stack.h"
#include "helper.h"
#include "program.h"

#define TAPE_LENGTH 30000 
#ifdef DEBUG
    #define dprintf(...) printf(__VA_ARGS__)
	#define dshow_stack(...) show_stack(__VA_ARGS__)
#else
    #define dprintf(...) ((void)0)
	#define dshow_stack(...) show_stack(__VA_ARGS__)
#endif
#define wprintf(...); if (!mute_warnings){ printf(__VA_ARGS__); }
#define FAULT_CODE -1
#define DEFAULT_CELLS_TO_DISPLAY 4

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
	"  • Provide no file for interactive mode\n"
	"  • -m must be provided before other options\n"
	"  • The order of options does not matter otherwise, but -l must be followed by a valid integer.\n"
	"  • The FILE argument is required and should point to a valid Brainfuck source file.\n";
const char* INTERACTIVE_MODE_MSG = "Brainfuck Interactive Mode\n"
	"\n"
	"Special Commands:\n"
	" - \"q\"    \tQuit interactive mode.\n"
	" - \"c\"    \tClear all memory cells in tape and reset pointer to 0.\n"
	" - \"s [n]\"\tShows (n or default) memory cells in tape\n";

void cleanup(){
	if (program_file != NULL) {
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

int main(int argc, char const *argv[])
{
	if (argc > 5){
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

    mem_strt = calloc(mem_length, sizeof(char));
	if (mem_strt == NULL){
		printf("[FAULT]: Could not allocate memory tape of size %d\n", mem_length);
		cleanup();
		exit(1);
	}

	while_stack = create_stack();

	if (file_name != NULL) {
		program_file = fopen(file_name, "r");
		if (program_file == NULL){
			printf("[FAULT]: Error opening file \"%s\".\n", file_name);
			exit(1);
		}

		parse_brainfuck(program_file, 0, mem_strt, mem_length, while_stack, mute_warnings);

	} else {
		printf(INTERACTIVE_MODE_MSG);

		int curr_ptr = 0;
		while (true){
			char line[1024];

			printf("\n$ ");
			fflush(stdout);

			if (fgets(line, sizeof(line), stdin) == NULL) {
			    break; // EOF or error
			}
			dprintf("[DEBUG]: Got Input \"%s\".\n", line);
			if (line[0] == 'q') {
				dprintf("[DEBUG]: Interactive mode quit.\n");
			    break;
			} else if (line[0] == 'c') {
			    memset(mem_strt, 0, mem_length);
			    curr_ptr = 0;
				dprintf("[DEBUG]: Interactive mode cell clear.\n");
			    continue; // skip parsing
			} else if (line[0] == 's') {
			    size_t cells_to_show = DEFAULT_CELLS_TO_DISPLAY;
			    char *end;
			    
			    // Skip leading spaces after 's'
			    char *arg = line + 1;
			    while (*arg == ' ' || *arg == '\t') arg++;

			    long value = strtol(arg, &end, 10);
			    if (end != arg && value >= 0) {
			        cells_to_show = (size_t)value;
			    }
			
				dprintf("[DEBUG]: Showing %zu cells from %zu.\n", cells_to_show, curr_ptr);
				display_cells(mem_strt, mem_length, curr_ptr, cells_to_show);
				continue;
			}

			// Now parse full line
			FILE *input_stream = fmemopen(line, strlen(line), "r");
			dprintf("[DEBUG]: Opened file stream with line.\n");

			if ((curr_ptr = parse_brainfuck(input_stream, curr_ptr, mem_strt, mem_length,
												 while_stack, mute_warnings)) == FAULT_CODE){
				break;
			}

			fclose(input_stream);
		}
	}

	cleanup();
	return 0;
}
