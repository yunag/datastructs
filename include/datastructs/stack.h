#ifndef YU_STACK_H
#define YU_STACK_H

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stack stack;

#define STACK_PUSH(S, elemT, elem)                                             \
  do {                                                                         \
    assert(sizeof(elemT) == stack_esize((S)));                                 \
    elemT __elem = (elem);                                                     \
    stack_push((S), &__elem);                                                  \
  } while (0)

#define STACK_TOP(S, elemT) *(elemT *)stack_top((S))

stack *stack_create(size_t size, size_t elemsize);
void stack_free(stack *stack);
void stack_push(stack *stack, const void *elem);
void stack_pop(stack *stack);
void *stack_top(stack *stack);
bool stack_empty(stack *stack);
bool stack_full(stack *stack);
size_t stack_size(stack *stack);
size_t stack_esize(stack *stack);

#ifdef __cplusplus
}
#endif

#endif // !YU_STACK_H
