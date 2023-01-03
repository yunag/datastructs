#include "datastructs/queue.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct queue {
  void *buffer;
  size_t size;
  size_t elemsize;
  size_t capacity;
  size_t front;
  size_t rear;
};

static inline void *queue_at(queue *q, size_t idx) {
  return &((uint8_t *)q->buffer)[q->elemsize * idx];
}

queue *queue_create(size_t size, size_t elemsize) {
  queue *q = malloc(sizeof(*q));
  if (q == NULL) {
    fprintf(stderr, "Failed to allocate memory for queue.\n");
    return NULL;
  }
  q->capacity = size;
  q->elemsize = elemsize;
  q->buffer = malloc(q->elemsize * q->capacity);
  if (q->buffer == NULL) {
    free(q);
    fprintf(stderr, "Failed to allocate memory for queue.\n");
    return NULL;
  }
  q->size = 0;
  q->front = 0;
  q->rear = q->capacity - 1;
  return q;
}

void queue_free(queue *q) {
  free(q->buffer);
  free(q);
}

bool queue_empty(queue *q) {
  assert(q != NULL);
  return q->size == 0;
}

bool queue_full(queue *q) {
  assert(q != NULL);
  return q->capacity == q->size;
}

static bool queue_resize(queue *q, size_t newsize) {
  assert(newsize > q->capacity);

  void *tmp = realloc(q->buffer, q->elemsize * newsize);
  if (tmp == NULL) {
    fprintf(stderr, "Failed to resize buffer for queue");
    return false;
  }
  q->buffer = tmp;
  q->capacity = newsize;
  return true;
}

void queue_push(queue *q, void *elem) {
  assert(q != NULL && elem != NULL);

  if (queue_full(q) && !queue_resize(q, q->capacity * 2)) {
    return;
  }

  q->size++;
  q->rear = (q->rear + 1) % q->capacity;
  memcpy(queue_at(q, q->rear), elem, q->elemsize);
}

void queue_pop(queue *q) {
  assert(q != NULL);

  if (queue_empty(q)) {
    return;
  }
  q->size--;
  q->front = (q->front + 1) % q->capacity;
}

void *queue_front(queue *q) {
  assert(q != NULL);

  if (queue_empty(q)) {
    return NULL;
  }
  return queue_at(q, q->front);
}

void *queue_back(queue *q) {
  assert(q != NULL);
  if (queue_empty(q)) {
    return NULL;
  }

  return queue_at(q, q->rear);
}
