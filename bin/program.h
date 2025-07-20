#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

#define FAULT_CODE -1

int parse_brainfuck(FILE *file, int ptr_start, void *mem_strt, int tape_length, 
	STACK wstack, bool mute_warnings);