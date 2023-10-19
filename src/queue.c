#include "datastructs/queue.h"
#include "datastructs/memory.h"

#include <assert.h>
#include <string.h>

struct queue {
  void *buffer; /* Buffer for storing elements */
  char *front;  /* Front index of the Queue */
  char *rear;   /* Rear index of the Queue */
  char *end;

  size_t size;     /* Size of the Queue */
  size_t capacity; /* Capacity of the Queue */
  size_t esize;    /* Size of a single element in the Queue */
};

queue *queue_create(size_t capacity, size_t elemsize) {
  assert(capacity > 0);
  assert(elemsize > 0);

  queue *q = _yu_allocator.allocate(sizeof(*q));
  if (!q) {
    return NULL;
  }

  q->buffer = _yu_allocator.allocate(elemsize * capacity);
  if (!q->buffer) {
    _yu_allocator.free(q);
    return NULL;
  }
  q->capacity = capacity;
  q->esize = elemsize;
  q->end = (char *)q->buffer + capacity * elemsize;
  q->size = 0;
  q->front = q->buffer;
  q->rear = q->buffer;
  return q;
}

void queue_destroy(queue *q) {
  if (q) {
    _yu_allocator.free(q->buffer);
    _yu_allocator.free(q);
  }
}

static bool queue_resize(queue *q, size_t newsize) {
  assert(newsize > q->size);

  char *buffer;
  size_t bufsize = newsize * q->esize;

  buffer = _yu_allocator.allocate(bufsize);
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
  _yu_allocator.free(q->buffer);

  q->front = buffer;
  q->rear = buffer + q->esize * q->size;
  q->end = buffer + bufsize;
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
