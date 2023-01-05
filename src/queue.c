#include "datastructs/queue.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct queue {
  void *buffer;    /* Buffer for storing elements */
  size_t front;    /* Front index of the Queue */
  size_t rear;     /* Rear index of the Queue */
  size_t size;     /* Size of the Queue */
  size_t capacity; /* Capacity of the Queue */
  size_t elemsize; /* Size of a single element in the Queue */
};

static inline void *queue_at(queue *q, size_t idx) {
  return &((uint8_t *)q->buffer)[q->elemsize * idx];
}

static inline void *buffer_at(void *buffer, size_t elemsize, size_t idx) {
  return &((uint8_t *)buffer)[elemsize * idx];
}

queue *queue_create(size_t size, size_t elemsize) {
  assert(size > 0 && elemsize > 0);
  queue *q = malloc(sizeof(*q));
  if (q == NULL) {
    fprintf(stderr, "Failed to allocate memory for queue.\n");
    return NULL;
  }
  q->buffer = malloc(elemsize * size);
  if (q->buffer == NULL) {
    free(q);
    fprintf(stderr, "Failed to allocate memory for queue.\n");
    return NULL;
  }
  q->capacity = size;
  q->elemsize = elemsize;
  q->size = 0;
  q->front = 0;
  q->rear = q->capacity - 1;
  return q;
}

void queue_free(queue *q) {
  assert(q != NULL);
  free(q->buffer);
  free(q);
}

static bool queue_resize(queue *q, size_t newsize) {
  assert(newsize > q->size);
  void *tmp;
  if (q->front <= q->rear) { /* Realloc case */
    tmp = realloc(q->buffer, newsize * q->elemsize);
    if (tmp == NULL) {
      fprintf(stderr, "Failed on buffer resize");
      return false;
    }
  } else {
    tmp = malloc(newsize * q->elemsize);
    if (tmp == NULL) {
      fprintf(stderr, "Failed on buffer resize");
      return false;
    }
    size_t nfront = q->size - q->front;
    /* copy from beginning up to rear */
    memcpy(tmp, q->buffer, (q->rear + 1) * q->elemsize);
    /* copy from front to the end */
    memcpy(buffer_at(tmp, q->elemsize, newsize - nfront), queue_at(q, q->front),
           nfront * q->elemsize);
    q->front = newsize - nfront;
    free(q->buffer);
  }
  q->buffer = tmp;
  q->capacity = newsize;
  return true;
}

void queue_push(queue *q, const void *elem) {
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

inline bool queue_empty(queue *q) {
  assert(q != NULL);
  return q->size == 0;
}

inline bool queue_full(queue *q) {
  assert(q != NULL);
  return q->capacity == q->size;
}

inline size_t queue_esize(queue *q) {
  assert(q != NULL);
  return q->elemsize;
}
