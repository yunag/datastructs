#include "datastructs/stack.h"
#include "datastructs/utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct stack {
  void *buffer;    /* Buffer for storing elements */
  size_t size;     /* Size of the Stack */
  size_t capacity; /* Capacity of the Stack */
  size_t esize;    /* Size of a single element in the Stack */
};

static inline void *stack_at(stack *s, size_t idx) {
  return &((char *)s->buffer)[s->esize * idx];
}

static bool stack_resize(stack *s, size_t newsize) {
  assert(s != NULL);
  assert(newsize > s->size);
  void *tmp = realloc(s->buffer, s->esize * newsize);
  if (tmp == NULL) {
    YU_LOG_ERROR("Failed to resize the stack to %zu\n", newsize);
    return false;
  }
  s->buffer = tmp;
  s->capacity = newsize;
  return true;
}

stack *stack_create(size_t size, size_t elemsize) {
  assert(size > 0);
  assert(elemsize > 0);

  stack *s = malloc(sizeof(*s));
  if (s == NULL) {
    YU_LOG_ERROR("Failed to allocate memory for stack\n");
    return NULL;
  }
  s->buffer = malloc(elemsize * size);
  if (s->buffer == NULL) {
    free(s);
    YU_LOG_ERROR("Failed to allocate memory for stack\n");
    return NULL;
  }
  s->capacity = size;
  s->esize = elemsize;
  s->size = 0;
  return s;
}

void stack_free(stack *s) {
  assert(s != NULL);
  free(s->buffer);
  free(s);
}

void stack_push(stack *s, const void *elem) {
  assert(s != NULL);
  assert(elem != NULL);

  if (stack_full(s) && !stack_resize(s, s->capacity * 2)) {
    return;
  }
  memcpy(stack_at(s, s->size++), elem, s->esize);
}

void stack_pop(stack *s) {
  assert(s != NULL);
  if (stack_empty(s)) {
    return;
  }
  s->size--;
}

void *stack_top(stack *s) {
  assert(s != NULL);
  if (stack_empty(s)) {
    return NULL;
  }
  return stack_at(s, s->size - 1);
}

bool stack_full(stack *s) {
  assert(s != NULL);
  return s->size == s->capacity;
}

bool stack_empty(stack *s) {
  assert(s != NULL);
  return s->size == 0;
}

size_t stack_size(stack *s) {
  assert(s != NULL);
  return s->size;
}

size_t stack_esize(stack *s) {
  assert(s != NULL);
  return s->esize;
}
