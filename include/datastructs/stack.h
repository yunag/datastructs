#ifndef YU_STACK_H
#define YU_STACK_H

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stack stack;

stack *stack_create(size_t capacity, size_t elemsize, free_fn free_elem);
void stack_free(stack *stack);
void stack_push(stack *stack, const void *elem);
void stack_pop(stack *stack);
void *stack_top(stack *stack);
bool stack_empty(stack *stack);
bool stack_full(stack *stack);
size_t stack_size(stack *stack);
size_t stack_esize(stack *stack);

#define STACK_PUSH(S, T, elem)                                                 \
  do {                                                                         \
    __typeof__(elem) __elem = (elem);                                          \
    stack_push(S, &__elem);                                                    \
  } while (0)

#define STACK_TOP(S, T) (*(T *)stack_top(S))
#define STACK_POP(S, ret)                                                      \
  do {                                                                         \
    ret = STACK_TOP(S, __typeof__(ret));                                       \
    stack_pop(S);                                                              \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif // !YU_STACK_H
