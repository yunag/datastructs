#include "datastructs/queue.h"
#include "datastructs/memory.h"

#include <assert.h>
#include <string.h>

struct queue {
  char *buffer; /* Buffer for storing elements */
  char *front;  /* Front index of the Queue */
  char *rear;   /* Rear index of the Queue */
  char *end;

  size_t num_items; /* Number of items in the Queue */
  size_t capacity;  /* Capacity of the Queue */
  size_t esize;     /* Size of a single element in the Queue */
};

queue *queue_create(size_t capacity, size_t elemsize) {
  assert(capacity > 0);
  assert(elemsize > 0);

  queue *q = yu_malloc(sizeof(*q));
  if (!q) {
    return NULL;
  }

  q->buffer = yu_malloc(elemsize * capacity);
  if (!q->buffer) {
    yu_free(q);
    return NULL;
  }

  q->front = q->buffer;
  q->rear = q->buffer;

  q->end = q->buffer + capacity * elemsize;

  q->num_items = 0;
  q->capacity = capacity;
  q->esize = elemsize;

  return q;
}

void queue_destroy(queue *q) {
  if (q) {
    yu_free(q->buffer);
    yu_free(q);
  }
}

static bool queue_resize(queue *q, size_t newsize) {
  assert(newsize > q->num_items);

  char *buffer;
  size_t bufsize = newsize * q->esize;

  buffer = yu_malloc(bufsize);
  if (!buffer) {
    return false;
  }

  if (q->front < q->rear) {
    memcpy(buffer, q->front, q->rear - q->front);
  } else {
    memcpy(buffer, q->front, q->end - q->front);
    memcpy(buffer + (q->end - q->front), q->buffer, q->rear - q->buffer);
  }

  yu_free(q->buffer);

  q->front = buffer;
  q->rear = buffer + q->esize * q->num_items;
  q->end = buffer + bufsize;

  q->buffer = buffer;
  q->capacity = newsize;

  return true;
}

bool queue_push(queue *q, const void *elem) {
  assert(q != NULL);
  assert(elem != NULL);

  if (queue_full(q) && !queue_resize(q, q->capacity * 2)) {
    return false;
  }

  if (q->rear == q->end) {
    q->rear = q->buffer;
  }

  memcpy(q->rear, elem, q->esize);
  q->rear += q->esize;
  q->num_items++;

  return true;
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
  q->num_items--;
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
  return !q->num_items;
}

bool queue_full(queue *q) {
  assert(q != NULL);
  return q->capacity == q->num_items;
}

size_t queue_size(queue *q) {
  assert(q != NULL);
  return q->num_items;
}

size_t queue_capacity(queue *q) {
  assert(q != NULL);
  return q->capacity;
}

size_t queue_esize(queue *q) {
  assert(q != NULL);
  return q->esize;
}
