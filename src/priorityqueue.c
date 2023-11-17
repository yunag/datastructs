#include "datastructs/macros.h"
#include "datastructs/memory.h"
#include "datastructs/priority_queue.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define HEAP_AT(node) (((char *)pq->heap + pq->esize * (node)))
#define PARENT(child) ((child - 1) >> 1)

#define LEFT_CHILD(node, heap, size) (heap + (((node)-heap) << 1) + size)
#define RIGHT_CHILD(node, heap, size) (heap + (((node)-heap + size) << 1))

#define LCHILD(parent) LEFT_CHILD(parent, heap, size)
#define RCHILD(left_child) (left_child + size)
#define HAS_PARENT(child) ((child) > 0)

struct priority_queue {
  char *heap; /* Node storage buffer */
  char *last;

  compare_fun cmp; /* Function for comparing two nodes */
  size_t size;     /* Size of the Priority Queue */
  size_t capacity; /* Capacity of the Priority Queue */
  size_t esize;    /* Size of a single element in the Priority Queue*/
};

static bool pq_resize(priority_queue *pq, size_t newsize) {
  assert(newsize > pq->size);

  char *tmp = yu_realloc(pq->heap, pq->esize * newsize);
  if (!tmp) {
    return false;
  }
  pq->heap = tmp;
  pq->last = tmp + pq->esize * pq->size;
  pq->capacity = newsize;
  return true;
}

static priority_queue *pq_init(size_t size, size_t capacity, size_t esize,
                               compare_fun cmp) {
  assert(capacity > 0);
  assert(esize > 0);
  assert(cmp != NULL);

  priority_queue *pq = yu_malloc(sizeof(*pq));
  if (!pq) {
    return NULL;
  }
  pq->heap = yu_malloc(capacity * esize);
  if (!pq->heap) {
    yu_free(pq);
    return NULL;
  }
  pq->capacity = capacity;
  pq->esize = esize;
  pq->cmp = cmp;
  pq->size = size;
  pq->last = pq->heap + pq->size * pq->esize;
  return pq;
}

priority_queue *pq_create(size_t capacity, size_t elemsize, compare_fun cmp) {
  return pq_init(0, capacity, elemsize, cmp);
}

priority_queue *pq_create_from_heap(const void *heap, size_t count,
                                    size_t elemsize, compare_fun cmp) {
  assert(heap != NULL);

  priority_queue *pq = pq_init(count, count, elemsize, cmp);
  memcpy(pq->heap, heap, count * elemsize);
  return pq;
}

priority_queue *pq_create_from_arr(const void *base, size_t count,
                                   size_t elemsize, compare_fun cmp) {
  assert(base != NULL);

  priority_queue *pq = pq_init(count, count, elemsize, cmp);
  memcpy(pq->heap, base, count * elemsize);
  heapify(pq->heap, count, elemsize, cmp);
  return pq;
}

void pq_destroy(priority_queue *pq) {
  if (pq) {
    yu_free(pq->heap);
    yu_free(pq);
  }
}

static void heapify_down(char *heap, char *last, char *node, size_t size,
                         compare_fun cmp) {
  char *cur = node;
  char *lch, *rch;
  while ((lch = LCHILD(cur)) < last) {
    if ((rch = RCHILD(lch)) < last && cmp(rch, lch) < 0) {
      lch = rch;
    }

    if (cmp(cur, lch) < 0) {
      break;
    }
    YU_BYTE_SWAP(lch, cur, size);
    cur = lch;
  }
}

void heapify(void *base, size_t count, size_t size, compare_fun cmp) {
  assert(base != NULL);
  assert(cmp != NULL);

  char *base_ptr = base;
  char *end = base_ptr + size * count;

  for (char *node = base_ptr + ((count >> 1) - 1) * size; node >= base_ptr;
       node -= size) {
    heapify_down(base, end, node, size, cmp);
  }
}

void pq_pushpop(priority_queue *pq, const void *elem) {
  assert(pq != NULL);
  assert(elem != NULL);

  memcpy(pq->heap, elem, pq->esize);
  heapify_down(pq->heap, pq->last, pq->heap, pq->esize, pq->cmp);
}

void pq_push(priority_queue *pq, const void *elem) {
  assert(pq != NULL);
  assert(elem != NULL);

  if (pq->size == pq->capacity && !pq_resize(pq, pq->capacity * 2)) {
    return;
  }

  size_t cur = pq->size;
  size_t par;

  memcpy(pq->last, elem, pq->esize);
  while (HAS_PARENT(cur) &&
         pq->cmp(HEAP_AT(cur), HEAP_AT(par = PARENT(cur))) < 0) {
    YU_BYTE_SWAP(HEAP_AT(par), HEAP_AT(cur), pq->esize);
    cur = par;
  }
  pq->size++;
  pq->last += pq->esize;
}

void pq_pop(priority_queue *pq) {
  assert(pq != NULL);

  if (pq_empty(pq)) {
    return;
  }
  pq->size--;

  /* Move last element to the top */
  memcpy(pq->heap, pq->last -= pq->esize, pq->esize);
  heapify_down(pq->heap, pq->last, pq->heap, pq->esize, pq->cmp);
}

bool pq_empty(priority_queue *pq) {
  assert(pq != NULL);
  return !pq->size;
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

const void *pq_heap(priority_queue *pq) {
  assert(pq != NULL);
  return pq->heap;
}
