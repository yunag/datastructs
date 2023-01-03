#include "datastructs/priority_queue.h"
#include "datastructs/utils.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct priority_queue {
  void *heap;
  cmp_fn cmp;
  size_t size;
  size_t capacity;
  size_t elemsize;
};

priority_queue *pq_create(size_t size, size_t elemsize, cmp_fn cmp) {
  assert(size > 0 && elemsize > 0);

  priority_queue *pq = malloc(sizeof(*pq));

  if (pq == NULL) {
    fprintf(stderr, "Failed to allocate memory for hash table.\n");
    return NULL;
  }

  pq->capacity = size;
  pq->elemsize = elemsize;
  pq->heap = malloc(pq->elemsize * pq->capacity);
  if (pq->heap == NULL) {
    free(pq);
    fprintf(stderr, "Failed to allocate memory for table.\n");
    return NULL;
  }
  pq->size = 0;
  pq->cmp = cmp;

  return pq;
}

void pq_free(priority_queue *pq) {
  free(pq->heap);
  free(pq);
}

/* Mappings:
 * Parent -> (child_idx - 1) / 2
 * Left Child -> 2 * Parent_idx + 1
 * Right Child -> 2 * Parent_idx + 2
 */
static inline size_t parent_idx(size_t child) { return (child - 1) / 2; }
static inline size_t lchild_idx(size_t parent) { return 2 * parent + 1; }
static inline size_t rchild_idx(size_t parent) { return 2 * parent + 2; }
static inline bool has_left_child(priority_queue *pq, size_t parent) {
  return lchild_idx(parent) < pq->size;
}
static inline bool has_right_child(priority_queue *pq, size_t parent) {
  return rchild_idx(parent) < pq->size;
}
static inline bool has_parent(size_t child) { return child >= 1; }
static inline void *heap_at(priority_queue *pq, size_t idx) {
  return &((uint8_t *)pq->heap)[pq->elemsize * idx];
}
static inline void *get_left_child(priority_queue *pq, size_t parent) {
  return heap_at(pq, lchild_idx(parent));
}
static inline void *get_right_child(priority_queue *pq, size_t parent) {
  return heap_at(pq, rchild_idx(parent));
}
static inline void *get_parent(priority_queue *pq, size_t child) {
  return heap_at(pq, parent_idx(child));
}

static bool pq_resize(priority_queue *pq, size_t newsize) {
  assert(newsize > pq->capacity);

  void *tmp = realloc(pq->heap, pq->elemsize * newsize);
  if (tmp == NULL) {
    fprintf(stderr, "Failed to resize buffer for queue");
    return false;
  }
  pq->heap = tmp;
  pq->capacity = newsize;
  return true;
}

void pq_push(priority_queue *pq, const void *elem) {
  if (pq->size == pq->capacity && !pq_resize(pq, pq->capacity * 2)) {
    return;
  }
  memcpy(heap_at(pq, pq->size), elem, pq->elemsize);

  size_t idx = pq->size++;
  while (has_parent(idx) &&
         pq->cmp(get_parent(pq, idx), heap_at(pq, idx)) > 0) {
    yu_byte_swap(get_parent(pq, idx), heap_at(pq, idx), pq->elemsize);
    idx = parent_idx(idx);
  }
}

size_t pq_size(priority_queue *pq) { return pq->size; }
bool pq_empty(priority_queue *pq) { return pq->size == 0; }
const void *pq_top(priority_queue *pq) { return pq->heap; }

void pq_pop(priority_queue *pq) {
  if (pq_empty(pq)) {
    return;
  }
  /* Move last element to the top */
  memcpy(pq->heap, heap_at(pq, --pq->size), pq->elemsize);

  size_t idx = 0;
  while (has_left_child(pq, idx)) {
    size_t child = lchild_idx(idx);
    if (has_right_child(pq, idx) &&
        pq->cmp(get_left_child(pq, idx), get_right_child(pq, idx)) > 0) {
      child = rchild_idx(idx);
    }

    if (pq->cmp(heap_at(pq, child), heap_at(pq, idx)) > 0) {
      break;
    }
    yu_byte_swap(heap_at(pq, child), heap_at(pq, idx), pq->elemsize);
    idx = child;
  }
}
