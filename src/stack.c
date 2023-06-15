#include "datastructs/stack.h"
#include "datastructs/functions.h"
#include "datastructs/memory.h"
#include "datastructs/types.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define STACK_EMPTY(s) ((s)->top == (s)->buffer)

struct stack {
  void *buffer;    /* Buffer for storing elements */
  char *top;       /* Top of the stack */
  free_fn free;    /* Function to free a value */
  size_t size;     /* Size of the Stack */
  size_t capacity; /* Capacity of the Stack */
  size_t esize;    /* Size of a single element in the Stack */
};

static bool stack_resize(stack *s, size_t newsize) {
  assert(s != NULL);
  assert(newsize >= 1);

  void *tmp = yu_realloc(s->buffer, s->esize * newsize);
  if (!tmp) {
    return false;
  }
  s->buffer = tmp;
  s->top = (char *)tmp + (s->esize * s->size);
  s->capacity = newsize;
  return true;
}

stack *stack_create(size_t capacity, size_t elemsize, free_fn vfree) {
  assert(capacity > 0);
  assert(elemsize > 0);

  stack *s = yu_allocate(sizeof(*s));
  if (!s) {
    return NULL;
  }
  s->buffer = yu_allocate(elemsize * capacity);
  if (!s->buffer) {
    free(s);
    return NULL;
  }
  s->top = s->buffer;
  s->free = vfree ? vfree : free_placeholder;
  s->capacity = capacity;
  s->esize = elemsize;
  s->size = 0;
  return s;
}

void stack_destroy(stack *s) {
  if (!s) {
    return;
  }
  if (s->free != free_placeholder) {
    while (!STACK_EMPTY(s)) {
      s->free(s->top -= s->esize);
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
  memcpy(s->top, elem, s->esize);
  s->top += s->esize;
  s->size++;
}

void stack_pop(stack *s) {
  assert(s != NULL);
  if (STACK_EMPTY(s)) {
    return;
  }
  s->free(s->top -= s->esize);
  s->size--;
}

void *stack_top(stack *s) {
  assert(s != NULL);
  if (STACK_EMPTY(s)) {
    return NULL;
  }
  return s->top - s->esize;
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
