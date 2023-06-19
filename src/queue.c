#include "datastructs/queue.h"
#include "datastructs/functions.h"
#include "datastructs/memory.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

  queue *q = yu_allocate(sizeof(*q));
  if (!q) {
    return NULL;
  }
  q->buffer = yu_allocate(elemsize * capacity);
  if (!q->buffer) {
    free(q);
    return NULL;
  }
  q->free = vfree ? vfree : free_placeholder;
  q->capacity = capacity;
  q->esize = elemsize;
  q->end = (char *)q->buffer + capacity * elemsize;
  q->size = 0;
  q->front = q->buffer;
  q->rear = q->buffer;
  return q;
}

void queue_destroy(queue *q) {
  if (!q) {
    return;
  }
  if (q->free != free_placeholder && !queue_empty(q)) {
    q->free(q->front);
    while ((q->front += q->esize) != q->rear) {
      if (q->front == q->end) {
        q->front = q->buffer;
      }
      q->free(q->front);
    }
  }
  free(q->buffer);
  free(q);
}

static bool queue_resize(queue *q, size_t newsize) {
  assert(newsize > q->size);
  char *buffer;
  size_t blocksize = newsize * q->esize;

  buffer = yu_allocate(blocksize);
  if (!buffer) {
    return false;
  }
  if (q->front < q->rear) {
    memcpy(buffer, q->front, q->rear - q->front);
  } else {
    memcpy(buffer, q->front, q->end - q->front);
    memcpy(buffer + (q->end - q->front), q->buffer,
           q->rear - (char *)q->buffer);
  }
  free(q->buffer);

  q->front = buffer;
  q->rear = buffer + q->esize * q->size;
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
  if (q->rear == q->end) {
    q->rear = q->buffer;
  }
  memcpy(q->rear, elem, q->esize);
  q->rear += q->esize;
  q->size++;
}

void queue_pop(queue *q) {
  assert(q != NULL);
  if (queue_empty(q)) {
    return;
  }
  q->free(q->front);
  q->front += q->esize;
  if (q->front == q->end) {
    q->front = q->buffer;
  }
  q->size--;
}

void *queue_front(queue *q) {
  assert(q != NULL);
  if (queue_empty(q)) {
    return NULL;
  }
  return q->front;
}

void *queue_back(queue *q) {
  assert(q != NULL);
  if (queue_empty(q)) {
    return NULL;
  }
  return q->rear - q->esize;
}

bool queue_empty(queue *q) {
  assert(q != NULL);
  return !q->size;
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
