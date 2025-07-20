#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void *str_append(char *str1, char* str2){
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    size_t new_length = len1 + len2 + 1;

    char *new_str = realloc(str1, new_length);
    if (!new_str) {
        return NULL;
    }

    strcat(new_str, str2);
    return new_str;
}


void display_cells(void *mem_cells, size_t num_cells, size_t curr_ptr, size_t N){
	size_t start = (curr_ptr > N) ? (curr_ptr - N) : 0;
	size_t end = (curr_ptr + N + 1 < num_cells) ? (curr_ptr + N + 1) : num_cells;

    char *cell_str = malloc(1);
    char *cell_display = malloc(1);
    
    if (cell_str == NULL || cell_display == NULL){
    	return;
    }

    cell_str[0] = '\0';
    cell_display[0] = '\0';
    cell_str = str_append(cell_str, "(value) ");
    cell_display = str_append(cell_display, "(cell#) ");

    if (cell_str == NULL || cell_display == NULL){
    	return;
    }

    char buffer[16];
    char *cells = (char *)mem_cells;

    for (size_t i = start; i < end; i++) {
    	switch (cells[i]){
    		case '\n': snprintf(buffer, sizeof(buffer), "| '\\n' |"); break;
	        case '\t': snprintf(buffer, sizeof(buffer), "| '\\t' |"); break;
	        case '\r': snprintf(buffer, sizeof(buffer), "| '\\r' |"); break;
	        case '\v': snprintf(buffer, sizeof(buffer), "| '\\v' |"); break;
	        case '\f': snprintf(buffer, sizeof(buffer), "| '\\f' |"); break;
	        case '\\': snprintf(buffer, sizeof(buffer), "| '\\\\' |"); break;
	        default:
		        if (isprint(cells[i])) {
		            snprintf(buffer, sizeof(buffer), "|  '%c' |", cells[i]);
		        } else {
		            snprintf(buffer, sizeof(buffer), "| 0x%02X |", (unsigned char)cells[i]);
		        }
    	}
        cell_str = str_append(cell_str, buffer);

        if (cell_str == NULL){
        	return;
        }


        snprintf(buffer, sizeof(buffer), "| %3d  |", i);
        if (i == curr_ptr) {
            cell_display = str_append(cell_display, "|   ^  |");
        } else {
            cell_display = str_append(cell_display, buffer);
        }

        if (cell_display == NULL){
        	return;
        }
    }

    for (size_t i = 0; i < strlen(cell_str); i++) printf("-");
    putchar('\n');

    puts(cell_str);
    puts(cell_display);

    for (size_t i = 0; i < strlen(cell_str); i++) printf("-");
    putchar('\n');

    free(cell_str);
    free(cell_display);
}