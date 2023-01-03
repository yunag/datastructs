#include "datastructs/stack.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stack {
  void *buffer;
  size_t size;
  size_t elemsize;
  size_t capacity;
};

static inline void *stack_at(stack *s, size_t idx) {
  return &((uint8_t *)s->buffer)[s->elemsize * idx];
}

stack *stack_create(size_t size, size_t elemsize) {
  assert(size > 0 && elemsize > 0);

  stack *s = malloc(sizeof(*s));
  if (s == NULL) {
    fprintf(stderr, "Failed to allocate memory for stack.\n");
    return NULL;
  }
  s->capacity = size;
  s->elemsize = elemsize;
  s->buffer = malloc(s->elemsize * s->capacity);
  if (s->buffer == NULL) {
    free(s);
    fprintf(stderr, "Failed to allocate memory for queue.\n");
    return NULL;
  }
  s->size = 0;
  return s;
}

void stack_free(stack *s) {
  assert(s != NULL);

  free(s->buffer);
  free(s);
}

bool stack_full(stack *s) {
  assert(s != NULL);
  return s->size == s->capacity;
}

static bool stack_resize(stack *s, size_t newsize) {
  assert(s != NULL && newsize > s->capacity);

  void *tmp = realloc(s->buffer, s->elemsize * newsize);
  if (tmp == NULL) {
    fprintf(stderr, "Failed to resize buffer for queue");
    return false;
  }
  s->buffer = tmp;
  s->capacity = newsize;
  return true;
}

void stack_push(stack *s, void *elem) {
  assert(s != NULL && elem != NULL);

  if (stack_full(s) && !stack_resize(s, s->capacity * 2)) {
    return;
  }
  memcpy(stack_at(s, s->size++), elem, s->elemsize);
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

bool stack_empty(stack *s) {
  assert(s != NULL);

  return s->size == 0;
}

size_t stack_size(stack *s) {
  assert(s != NULL);

  return s->size;
}
