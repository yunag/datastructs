#include "datastructs/queue.h"
#include "datastructs/functions.h"
#include "datastructs/macros.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_EMPTY(q) ((q)->size == 0)

struct queue {
  void *buffer; /* Buffer for storing elements */
  char *front;  /* Front index of the Queue */
  char *rear;   /* Rear index of the Queue */
  char *end;
  size_t size;     /* Size of the Queue */
  size_t capacity; /* Capacity of the Queue */
  size_t esize;    /* Size of a single element in the Queue */

  free_fn free;
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
  q->end = (char *)q->buffer + capacity * elemsize;
  q->size = 0;
  q->front = q->buffer;
  q->rear = q->end - elemsize;
  return q;
}

void queue_destroy(queue *q) {
  if (!q) {
    return;
  }
  if (q->free != free_placeholder) {
    while (!QUEUE_EMPTY(q)) {
      q->free(q->front);
      q->size--;
      q->front += q->esize;
      if (q->front == q->end) {
        q->front = q->buffer;
      }
    }
  }
  free(q->buffer);
  free(q);
}

static bool queue_resize(queue *q, size_t newsize) {
  assert(newsize > q->size);
  char *buffer;
  size_t blocksize = newsize * q->esize;

  buffer = malloc(blocksize);
  if (!buffer) {
    YU_LOG_ERROR("Failed to resize the queue to %zu", newsize);
    return false;
  }
  if (q->front <= q->rear) {
    memcpy(buffer, q->front, q->rear - q->front + q->esize);
  } else {
    memcpy(buffer, q->front, q->end - q->front);
    memcpy(buffer + (q->end - q->front), q->buffer,
           q->rear - (char *)q->buffer + q->esize);
  }
  free(q->buffer);

  q->front = buffer;
  q->rear = buffer + q->esize * (q->size - 1);
  q->end = buffer + blocksize;
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
  q->rear = q->rear + q->esize;
  if (q->rear == q->end) {
    q->rear = q->buffer;
  }
  memcpy(q->rear, elem, q->esize);
}

void queue_pop(queue *q) {
  assert(q != NULL);
  if (QUEUE_EMPTY(q)) {
    return;
  }
  q->free(q->front);
  q->size--;
  q->front += q->esize;
  if (q->front == q->end) {
    q->front = q->buffer;
  }
}

void *queue_front(queue *q) {
  assert(q != NULL);
  if (QUEUE_EMPTY(q)) {
    return NULL;
  }
  return q->front;
}

void *queue_back(queue *q) {
  assert(q != NULL);
  if (QUEUE_EMPTY(q)) {
    return NULL;
  }
  return q->rear;
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
