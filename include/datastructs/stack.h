#ifndef YU_STACK_H
#define YU_STACK_H

#include "types.h"
#include <stdbool.h>

typedef struct stack stack;

stack *stack_create(size_t size, size_t elemsize);
void stack_free(stack *stack);
void stack_push(stack *stack, void *elem);
void stack_pop(stack *stack);
void *stack_top(stack *stack);
bool stack_empty(stack *stack);
bool stack_full(stack *stack);
size_t stack_size(stack *stack);

#endif // !YU_STACK_H
