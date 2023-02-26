#include "datastructs/priority_queue.h"
#include "datastructs/utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct priority_queue {
  void *heap;      /* Node storage buffer */
  cmp_fn cmp;      /* Function for comparing two nodes */
  size_t size;     /* Size of the Priority Queue */
  size_t capacity; /* Capacity of the Priority Queue */
  size_t esize;    /* Size of a single element in the Priority Queue*/
};

static inline size_t parent_idx(size_t child) { return (child - 1) / 2; }
static inline size_t leftc_idx(size_t parent) { return 2 * parent + 1; }
static inline size_t rightc_idx(size_t parent) { return 2 * parent + 2; }

static inline bool has_leftc(const priority_queue *pq, size_t parent) {
  return leftc_idx(parent) < pq->size;
}
static inline bool has_rightc(const priority_queue *pq, size_t parent) {
  return rightc_idx(parent) < pq->size;
}
static inline bool has_parent(size_t child) { return child >= 1; }
static inline void *heap_at(const priority_queue *pq, size_t idx) {
  return &((char *)pq->heap)[pq->esize * idx];
}
static inline void *leftc(const priority_queue *pq, size_t parent) {
  return heap_at(pq, leftc_idx(parent));
}
static inline void *rightc(const priority_queue *pq, size_t parent) {
  return heap_at(pq, rightc_idx(parent));
}
static inline void *parent(const priority_queue *pq, size_t child) {
  return heap_at(pq, parent_idx(child));
}

static bool pq_resize(priority_queue *pq, const size_t newsize) {
  assert(newsize > pq->size);
  void *tmp = realloc(pq->heap, pq->esize * newsize);
  if (tmp == NULL) {
    YU_LOG_ERROR("Failed to resize buffer for queue\n");
    return false;
  }
  pq->heap = tmp;
  pq->capacity = newsize;
  return true;
}

priority_queue *pq_create(size_t size, size_t elemsize, cmp_fn cmp) {
  assert(size > 0 && elemsize > 0 && cmp != NULL);
  priority_queue *pq = malloc(sizeof(*pq));
  if (pq == NULL) {
    YU_LOG_ERROR("Failed to allocate memory for priority queue\n");
    return NULL;
  }
  pq->heap = malloc(elemsize * size);
  if (pq->heap == NULL) {
    free(pq);
    YU_LOG_ERROR("Failed to allocate memory for heap\n");
    return NULL;
  }
  pq->capacity = size;
  pq->esize = elemsize;
  pq->size = 0;
  pq->cmp = cmp;
  return pq;
}

void pq_free(priority_queue *pq) {
  assert(pq != NULL);
  free(pq->heap);
  free(pq);
}

void pq_push(priority_queue *pq, const void *elem) {
  assert(pq != NULL && elem != NULL);
  if (pq->size == pq->capacity && !pq_resize(pq, pq->capacity * 2)) {
    return;
  }
  memcpy(heap_at(pq, pq->size), elem, pq->esize);
  size_t p = pq->size++;
  while (has_parent(p) && pq->cmp(heap_at(pq, p), parent(pq, p)) < 0) {
    YU_BYTE_SWAP(parent(pq, p), heap_at(pq, p), pq->esize);
    p = parent_idx(p);
  }
}

void pq_pop(priority_queue *pq) {
  if (pq_empty(pq)) {
    return;
  }
  /* Move last element to the top */
  memcpy(pq->heap, heap_at(pq, --pq->size), pq->esize);

  size_t p = 0;
  while (has_leftc(pq, p)) {
    size_t child = leftc_idx(p);
    if (has_rightc(pq, p) && pq->cmp(rightc(pq, p), leftc(pq, p)) < 0) {
      child = rightc_idx(p);
    }

    if (pq->cmp(heap_at(pq, p), heap_at(pq, child)) < 0) {
      break;
    }
    YU_BYTE_SWAP(heap_at(pq, child), heap_at(pq, p), pq->esize);
    p = child;
  }
}

bool pq_empty(priority_queue *pq) {
  assert(pq != NULL);
  return pq->size == 0;
}

const void *pq_top(priority_queue *pq) {
  assert(pq != NULL);
  if (pq_empty(pq)) {
    return NULL;
  }
  return pq->heap;
}

size_t pq_size(priority_queue *pq) {
  assert(pq != NULL);
  return pq->size;
}

size_t pq_esize(priority_queue *pq) {
  assert(pq != NULL);
  return pq->esize;
}
