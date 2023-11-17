#include "datastructs/stack.h"
#include "datastructs/memory.h"
#include "datastructs/types.h"

#include <assert.h>
#include <string.h>

struct stack {
  void *buffer; /* Buffer for storing elements */
  char *top;    /* Top of the stack */
  char *end;

  size_t esize; /* Size of a single element in the Stack */
};

static bool stack_resize(stack *s, size_t newsize) {
  assert(s != NULL);
  assert(newsize > 0);

  char *tmp = yu_realloc(s->buffer, s->esize * newsize);
  if (!tmp) {
    return false;
  }
  s->top = tmp + (s->top - (char *)s->buffer);
  s->buffer = tmp;
  s->end = tmp + newsize * s->esize;
  return true;
}

stack *stack_create(size_t capacity, size_t elemsize) {
  assert(capacity > 0);
  assert(elemsize > 0);

  stack *s = yu_malloc(sizeof(*s));
  if (!s) {
    return NULL;
  }
  s->buffer = yu_malloc(elemsize * capacity);
  if (!s->buffer) {
    yu_free(s);
    return NULL;
  }
  s->top = s->buffer;
  s->end = (char *)s->buffer + elemsize * capacity;
  s->esize = elemsize;
  return s;
}

void stack_destroy(stack *s) {
  if (s) {
    yu_free(s->buffer);
    yu_free(s);
  }
}

void stack_push(stack *s, const void *elem) {
  assert(s != NULL);
  assert(elem != NULL);

  if (stack_full(s) && !stack_resize(s, stack_size(s) * 2)) {
    return;
  }
  memcpy(s->top, elem, s->esize);
  s->top += s->esize;
}

void stack_pop(stack *s) {
  assert(s != NULL);

  if (stack_empty(s)) {
    return;
  }
  s->top -= s->esize;
}

void *stack_top(stack *s) {
  assert(s != NULL);

  if (stack_empty(s)) {
    return NULL;
  }
  return s->top - s->esize;
}

bool stack_full(stack *s) {
  assert(s != NULL);
  return s->top == s->end;
}

bool stack_empty(stack *s) {
  assert(s != NULL);
  return s->top == s->buffer;
}

size_t stack_size(stack *s) {
  assert(s != NULL);
  return (s->top - (char *)s->buffer) / s->esize;
}

size_t stack_esize(stack *s) {
  assert(s != NULL);
  return s->esize;
}
