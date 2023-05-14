#include "datastructs/macros.h"
#include "datastructs/priority_queue.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef uint64_t pqnode;

#define PARENT(child) (((child)-1) >> 1)
#define LCHILD(parent) (((parent) << 1) + 1)
#define RCHILD(parent) (((parent) << 1) + 2)
#define HEAP_AT(node) ((void *)&((char *)pq->heap)[pq->esize * (node)])
#define HAS_PARENT(child) ((child) > 0)

#define PQ_EMPTY(pq) ((pq)->size == 0)

struct priority_queue {
  void *heap;      /* Node storage buffer */
  cmp_fn cmp;      /* Function for comparing two nodes */
  size_t size;     /* Size of the Priority Queue */
  size_t capacity; /* Capacity of the Priority Queue */
  size_t esize;    /* Size of a single element in the Priority Queue*/
};

static bool pq_resize(priority_queue *pq, size_t newsize) {
  assert(newsize > pq->size);
  void *tmp = realloc(pq->heap, pq->esize * newsize);
  if (!tmp) {
    YU_LOG_ERROR("Failed to resize the priority queue to %zu", newsize);
    return false;
  }
  pq->heap = tmp;
  pq->capacity = newsize;
  return true;
}

#define PQ_INIT(pq, nsize, ncapacity, nelemsize, ncmp)                         \
  do {                                                                         \
    assert(ncapacity > 0);                                                     \
    assert(nelemsize > 0);                                                     \
    assert(ncmp != NULL);                                                      \
                                                                               \
    pq = malloc(sizeof(*pq));                                                  \
    if (!pq) {                                                                 \
      YU_LOG_ERROR("Failed to allocate memory for priority queue");            \
      return NULL;                                                             \
    }                                                                          \
    pq->heap = malloc(ncapacity * nelemsize);                                  \
    if (!pq->heap) {                                                           \
      free(pq);                                                                \
      YU_LOG_ERROR("Failed to allocate memory for heap");                      \
      return NULL;                                                             \
    }                                                                          \
    pq->capacity = ncapacity;                                                  \
    pq->esize = nelemsize;                                                     \
    pq->cmp = ncmp;                                                            \
    pq->size = nsize;                                                          \
  } while (0)

priority_queue *pq_create(size_t capacity, size_t elemsize, cmp_fn cmp) {
  priority_queue *pq;
  PQ_INIT(pq, 0, capacity, elemsize, cmp);
  return pq;
}

priority_queue *pq_create_from_heap(const void *heap, size_t count,
                                    size_t elemsize, cmp_fn cmp) {
  priority_queue *pq;
  PQ_INIT(pq, count, count, elemsize, cmp);
  memcpy(pq->heap, heap, count * elemsize);
  return pq;
}

priority_queue *pq_create_from_arr(const void *base, size_t count,
                                   size_t elemsize, cmp_fn cmp) {
  priority_queue *pq;
  PQ_INIT(pq, count, count, elemsize, cmp);
  memcpy(pq->heap, base, count * elemsize);
  heapify(pq->heap, count, elemsize, cmp);
  return pq;
}

void pq_destroy(priority_queue *pq) {
  if (pq) {
    free(pq->heap);
    free(pq);
  }
}

void pq_push(priority_queue *pq, const void *elem) {
  assert(pq != NULL);
  assert(elem != NULL);

  if (pq->size == pq->capacity && !pq_resize(pq, pq->capacity * 2)) {
    return;
  }
  memcpy(HEAP_AT(pq->size), elem, pq->esize);
  pqnode cur = pq->size++;
  pqnode par;
  while (HAS_PARENT(cur) &&
         pq->cmp(HEAP_AT(cur), HEAP_AT((par = PARENT(cur)))) < 0) {
    YU_BYTE_SWAP(HEAP_AT(par), HEAP_AT(cur), pq->esize);
    cur = par;
  }
}

void pq_pop(priority_queue *pq) {
  assert(pq != NULL);
  if (PQ_EMPTY(pq)) {
    return;
  }
  /* Move last element to the top */
  memcpy(pq->heap, HEAP_AT(--pq->size), pq->esize);

  pqnode cur = 0;
  pqnode lch, rch;
  while ((lch = LCHILD(cur)) < pq->size) {
    if ((rch = RCHILD(cur)) < pq->size &&
        pq->cmp(HEAP_AT(rch), HEAP_AT(lch)) < 0) {
      lch = rch;
    }

    if (pq->cmp(HEAP_AT(cur), HEAP_AT(lch)) < 0) {
      break;
    }
    YU_BYTE_SWAP(HEAP_AT(lch), HEAP_AT(cur), pq->esize);
    cur = lch;
  }
}

bool pq_empty(priority_queue *pq) {
  assert(pq != NULL);
  return PQ_EMPTY(pq);
}

const void *pq_top(priority_queue *pq) {
  assert(pq != NULL);
  if (PQ_EMPTY(pq)) {
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

void heapify(void *base, size_t count, size_t size, cmp_fn cmp) {
  assert(base != NULL);
  assert(cmp != NULL);

#define BUF_AT(index) ((void *)&((char *)base)[size * index])
  for (int64_t i = (count >> 1) - 1; i >= 0; --i) {
    pqnode cur = i;
    pqnode lch, rch;
    while ((lch = LCHILD(cur)) < count) {
      if ((rch = RCHILD(cur)) < count && cmp(BUF_AT(rch), BUF_AT(lch)) < 0) {
        lch = rch;
      }

      if (cmp(BUF_AT(cur), BUF_AT(lch)) < 0) {
        break;
      }
      YU_BYTE_SWAP(BUF_AT(lch), BUF_AT(cur), size);
      cur = lch;
    }
  }
#undef BUF_AT
}
