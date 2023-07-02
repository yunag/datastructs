#ifndef YU_STACK_H
#define YU_STACK_H

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stack stack;

stack *stack_create(size_t initial_capacity, size_t elemsize);
void stack_destroy(stack *stack);
void stack_push(stack *stack, const void *elem);
void stack_pop(stack *stack);
void *stack_top(stack *stack);
bool stack_empty(stack *stack);
bool stack_full(stack *stack);
size_t stack_size(stack *stack);
size_t stack_esize(stack *stack);

#define STACK_PUSH(stack, elem)                                                \
  do {                                                                         \
    __typeof__(elem) __elem = (elem);                                          \
    stack_push(stack, &__elem);                                                \
  } while (0)

#define STACK_TOP(stack, type) (*(type *)stack_top(stack))
#define STACK_POP(stack, return_value)                                         \
  do {                                                                         \
    return_value = STACK_TOP(stack, __typeof__(return_value));                 \
    stack_pop(stack);                                                          \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif // !YU_STACK_H
