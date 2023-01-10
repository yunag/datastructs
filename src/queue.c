#include "datastructs/queue.h"
#include "datastructs/utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct queue {
  void *buffer;    /* Buffer for storing elements */
  size_t front;    /* Front index of the Queue */
  size_t rear;     /* Rear index of the Queue */
  size_t size;     /* Size of the Queue */
  size_t capacity; /* Capacity of the Queue */
  size_t esize;    /* Size of a single element in the Queue */
};

static inline void *queue_at(queue *q, size_t idx) {
  return &((char *)q->buffer)[q->esize * idx];
}

static inline void *buffer_at(void *buffer, size_t esize, size_t idx) {
  return &((char *)buffer)[esize * idx];
}

queue *queue_create(size_t size, size_t elemsize) {
  assert(size > 0 && elemsize > 0);
  queue *q = malloc(sizeof(*q));
  if (q == NULL) {
    YU_LOG_ERROR("Failed to allocate memory for queue");
    return NULL;
  }
  q->buffer = malloc(elemsize * size);
  if (q->buffer == NULL) {
    free(q);
    YU_LOG_ERROR("Failed to allocate memory for queue");
    return NULL;
  }
  q->capacity = size;
  q->esize = elemsize;
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
  void *buffer;
  if (q->front <= q->rear) { /* Realloc case */
    buffer = realloc(q->buffer, newsize * q->esize);
    if (buffer == NULL) {
      YU_LOG_ERROR("Failed on buffer resize");
      return false;
    }
  } else {
    buffer = malloc(newsize * q->esize);
    if (buffer == NULL) {
      YU_LOG_ERROR("Failed on buffer resize");
      return false;
    }
    size_t nfront = q->size - q->front;
    memcpy(buffer, queue_at(q, q->front), nfront * q->esize);
    memcpy(buffer_at(buffer, q->esize, nfront), q->buffer,
           (q->rear + 1) * q->esize);
    q->rear = q->size - 1;
    q->front = 0;
    free(q->buffer);
  }
  q->buffer = buffer;
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
  memcpy(queue_at(q, q->rear), elem, q->esize);
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

bool queue_empty(queue *q) {
  assert(q != NULL);
  return q->size == 0;
}

bool queue_full(queue *q) {
  assert(q != NULL);
  return q->capacity == q->size;
}

size_t queue_size(queue *q) {
  assert(q != NULL);
  return q->size;
}

size_t queue_esize(queue *q) {
  assert(q != NULL);
  return q->esize;
}
