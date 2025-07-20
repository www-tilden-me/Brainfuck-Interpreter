// @file: defines a stack of size_t defined for brainfuck interpreter usage
#include <stdlib.h>
#include <stdbool.h>
#include<stdio.h>

typedef struct stack_elem_t{
	struct stack_elem_t *next;
	size_t value;
} stack_elem_t;

typedef struct stack_t
{
	stack_elem_t *start;
} stack_t;

typedef stack_t* STACK;

STACK create_stack(){
	STACK stack = malloc(sizeof(stack_t));
	if (stack == NULL){
		return NULL;
	}

	stack->start = NULL;
	return stack;
}

bool is_stack_empty(STACK stack){
	if (stack == NULL || stack->start == NULL){
		return true;
	}
	return false;
}

void stack_push(STACK stack, size_t value){
	if (stack == NULL){
		return;
	}

	stack_elem_t *new = malloc(sizeof(stack_elem_t));
	new->value = value;

	if (stack->start == NULL){
		new->next = NULL;
		stack->start = new;
	} else {
		new->next = stack->start;
		stack->start = new;
	}
}

bool stack_pop(STACK stack, size_t *value){
	if (stack == NULL || stack->start == NULL){
		return false;
	}

	stack_elem_t *elem = stack->start;
	stack->start = elem->next;

	*value = elem->value;

	free(elem);
	return true;
}

void cleanup_stack(STACK stack){
	if (stack == NULL) return;

	stack_elem_t *curr = stack->start;
	while(curr != NULL){
		stack_elem_t *tmp = curr;
		curr = curr->next;
		free(tmp);
	}

	free(stack);
}

void show_stack(STACK stack){
	stack_elem_t *curr = stack->start;

	printf("STACK: ");
	while(curr != NULL){
		printf("%u -> ", curr->value);
		curr = curr->next;
	}
	printf("(null)\n");
}