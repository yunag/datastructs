#include "datastructs/queue.h"
#include "datastructs/macros.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_AT(index) ((void *)&((char *)q->buffer)[q->esize * (index)])
#define QUEUE_EMPTY(q) ((q)->size == 0)

struct queue {
  void *buffer; /* Buffer for storing elements */
  free_fn free;
  size_t front;    /* Front index of the Queue */
  size_t rear;     /* Rear index of the Queue */
  size_t size;     /* Size of the Queue */
  size_t capacity; /* Capacity of the Queue */
  size_t esize;    /* Size of a single element in the Queue */
};

queue *queue_create(size_t capacity, size_t elemsize, free_fn vfree) {
  assert(capacity > 0);
  assert(elemsize > 0);

  queue *q = malloc(sizeof(*q));
  if (!q) {
    YU_LOG_ERROR("Failed to allocate memory for queue");
    return NULL;
  }
  q->buffer = malloc(elemsize * capacity);
  if (!q->buffer) {
    free(q);
    YU_LOG_ERROR("Failed to allocate memory for queue");
    return NULL;
  }
  q->free = vfree ? vfree : free_placeholder;
  q->capacity = capacity;
  q->esize = elemsize;
  q->size = 0;
  q->front = 0;
  q->rear = q->capacity - 1;
  return q;
}

void queue_destroy(queue *q) {
  if (!q) {
    return;
  }
  if (q->free != free_placeholder) {
    while (!QUEUE_EMPTY(q)) {
      q->free(QUEUE_AT(q->front));
      q->size--;
      q->front = (q->front + 1) % q->capacity;
    }
  }
  free(q->buffer);
  free(q);
}

static bool queue_resize(queue *q, size_t newsize) {
  assert(newsize > q->size);
  char *buffer;
  if (q->front <= q->rear) { /* Realloc case */
    buffer = realloc(q->buffer, newsize * q->esize);
    if (!buffer) {
      YU_LOG_ERROR("Failed to resize the queue to %zu", newsize);
      return false;
    }
  } else {
    buffer = malloc(newsize * q->esize);
    if (!buffer) {
      YU_LOG_ERROR("Failed to resize the queue to %zu", newsize);
      return false;
    }
    size_t nfront = q->size - q->front;
    memcpy(buffer, QUEUE_AT(q->front), nfront * q->esize);
    memcpy(&buffer[nfront * q->esize], q->buffer, (q->rear + 1) * q->esize);
    q->rear = q->size - 1;
    q->front = 0;
    free(q->buffer);
  }
  q->buffer = buffer;
  q->capacity = newsize;
  return true;
}

void queue_push(queue *q, const void *elem) {
  assert(q != NULL);
  assert(elem != NULL);
  if (queue_full(q) && !queue_resize(q, q->capacity * 2)) {
    return;
  }
  q->size++;
  q->rear = (q->rear + 1) % q->capacity;
  memcpy(QUEUE_AT(q->rear), elem, q->esize);
}

void queue_pop(queue *q) {
  assert(q != NULL);
  if (QUEUE_EMPTY(q)) {
    return;
  }
  q->free(QUEUE_AT(q->front));
  q->size--;
  q->front = (q->front + 1) % q->capacity;
}

void *queue_front(queue *q) {
  assert(q != NULL);
  if (QUEUE_EMPTY(q)) {
    return NULL;
  }
  return QUEUE_AT(q->front);
}

void *queue_back(queue *q) {
  assert(q != NULL);
  if (QUEUE_EMPTY(q)) {
    return NULL;
  }
  return QUEUE_AT(q->rear);
}

bool queue_empty(queue *q) {
  assert(q != NULL);
  return QUEUE_EMPTY(q);
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
