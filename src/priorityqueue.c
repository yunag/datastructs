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
  size_t elemsize; /* Size of a single element in the Priority Queue*/
};

static inline size_t parent_idx(size_t child) { return (child - 1) / 2; }
static inline size_t lchild_idx(size_t parent) { return 2 * parent + 1; }
static inline size_t rchild_idx(size_t parent) { return 2 * parent + 2; }
static inline bool has_lchild(priority_queue *pq, const size_t parent) {
  return lchild_idx(parent) < pq->size;
}
static inline bool has_rchild(priority_queue *pq, const size_t parent) {
  return rchild_idx(parent) < pq->size;
}
static inline bool has_parent(size_t child) { return child >= 1; }
static inline void *heap_at(priority_queue *pq, const size_t idx) {
  return &((char *)pq->heap)[pq->elemsize * idx];
}
static inline void *left_child(priority_queue *pq, const size_t parent) {
  return heap_at(pq, lchild_idx(parent));
}
static inline void *right_child(priority_queue *pq, const size_t parent) {
  return heap_at(pq, rchild_idx(parent));
}
static inline void *parent(priority_queue *pq, const size_t child) {
  return heap_at(pq, parent_idx(child));
}

static bool pq_resize(priority_queue *pq, const size_t newsize) {
  assert(newsize > pq->size);
  void *tmp = realloc(pq->heap, pq->elemsize * newsize);
  if (tmp == NULL) {
    yu_log_error("Failed to resize buffer for queue");
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
    yu_log_error("Failed to allocate memory for priority queue");
    return NULL;
  }
  pq->heap = malloc(elemsize * size);
  if (pq->heap == NULL) {
    free(pq);
    yu_log_error("Failed to allocate memory for heap");
    return NULL;
  }
  pq->capacity = size;
  pq->elemsize = elemsize;
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
  memcpy(heap_at(pq, pq->size), elem, pq->elemsize);
  size_t idx = pq->size++;
  while (has_parent(idx) && pq->cmp(parent(pq, idx), heap_at(pq, idx)) > 0) {
    yu_byte_swap(parent(pq, idx), heap_at(pq, idx), pq->elemsize);
    idx = parent_idx(idx);
  }
}

void pq_pop(priority_queue *pq) {
  if (pq_empty(pq)) {
    return;
  }
  /* Move last element to the top */
  memcpy(pq->heap, heap_at(pq, --pq->size), pq->elemsize);

  size_t idx = 0;
  while (has_lchild(pq, idx)) {
    size_t swap_child = lchild_idx(idx);
    if (has_rchild(pq, idx) &&
        pq->cmp(left_child(pq, idx), right_child(pq, idx)) > 0) {
      swap_child = rchild_idx(idx);
    }

    if (pq->cmp(heap_at(pq, swap_child), heap_at(pq, idx)) > 0) {
      break;
    }
    yu_byte_swap(heap_at(pq, swap_child), heap_at(pq, idx), pq->elemsize);
    idx = swap_child;
  }
}

bool pq_empty(priority_queue *pq) {
  assert(pq != NULL);
  return pq->size == 0;
}

const void *pq_top(priority_queue *pq) {
  assert(pq != NULL);
  return pq->heap;
}

size_t pq_size(priority_queue *pq) {
  assert(pq != NULL);
  return pq->size;
}

size_t pq_esize(priority_queue *pq) {
  assert(pq != NULL);
  return pq->elemsize;
}
