typedef struct stack_t* STACK;

STACK create_stack(void);
bool is_stack_empty(STACK stack);
void stack_push(STACK stack, size_t value);
bool stack_pop(STACK stack, size_t *value);
void cleanup_stack(STACK stack);
void show_stack(STACK stack);