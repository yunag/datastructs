#include "datastructs/stack.h"
#include "datastructs/macros.h"
#include "datastructs/types.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define STACK_AT(index) ((void *)&((char *)s->buffer)[s->esize * (index)])
#define STACK_EMPTY(s) ((s)->size == 0)

struct stack {
  void *buffer;    /* Buffer for storing elements */
  free_fn free;    /* Function to free a value */
  size_t size;     /* Size of the Stack */
  size_t capacity; /* Capacity of the Stack */
  size_t esize;    /* Size of a single element in the Stack */
};

static bool stack_resize(stack *s, size_t newsize) {
  assert(s != NULL);
  assert(newsize > s->size);
  void *tmp = realloc(s->buffer, s->esize * newsize);
  if (!tmp) {
    YU_LOG_ERROR("Failed to resize the stack to %zu", newsize);
    return false;
  }
  s->buffer = tmp;
  s->capacity = newsize;
  return true;
}

stack *stack_create(size_t capacity, size_t elemsize, free_fn vfree) {
  assert(capacity > 0);
  assert(elemsize > 0);

  stack *s = malloc(sizeof(*s));
  if (!s) {
    YU_LOG_ERROR("Failed to allocate memory for stack");
    return NULL;
  }
  s->buffer = malloc(elemsize * capacity);
  if (!s->buffer) {
    free(s);
    YU_LOG_ERROR("Failed to allocate memory for stack");
    return NULL;
  }
  s->free = vfree ? vfree : free_placeholder;
  s->capacity = capacity;
  s->esize = elemsize;
  s->size = 0;
  return s;
}

void stack_free(stack *s) {
  assert(s != NULL);
  if (s->free != free_placeholder) {
    while (!STACK_EMPTY(s)) {
      s->free(STACK_AT(--s->size));
    }
  }
  free(s->buffer);
  free(s);
}

void stack_push(stack *s, const void *elem) {
  assert(s != NULL);
  assert(elem != NULL);

  if (stack_full(s) && !stack_resize(s, s->capacity * 2)) {
    return;
  }
  memcpy(STACK_AT(s->size++), elem, s->esize);
}

void stack_pop(stack *s) {
  assert(s != NULL);
  if (STACK_EMPTY(s)) {
    return;
  }
  s->free(STACK_AT(--s->size));
}

void *stack_top(stack *s) {
  assert(s != NULL);
  if (STACK_EMPTY(s)) {
    return NULL;
  }
  return STACK_AT(s->size - 1);
}

bool stack_full(stack *s) {
  assert(s != NULL);
  return s->size == s->capacity;
}

bool stack_empty(stack *s) {
  assert(s != NULL);
  return STACK_EMPTY(s);
}

size_t stack_size(stack *s) {
  assert(s != NULL);
  return s->size;
}

size_t stack_esize(stack *s) {
  assert(s != NULL);
  return s->esize;
}
