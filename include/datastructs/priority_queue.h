#ifndef YU_PRIORITY_QUEUE_H
#define YU_PRIORITY_QUEUE_H

#include <stdbool.h>
#include <stddef.h>

#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct priority_queue priority_queue;
typedef bool (*pq_less_fun)(const void *, const void *);

priority_queue *pq_create(size_t capacity, size_t elemsize, pq_less_fun less);
priority_queue *pq_create_from_heap(const void *heap, size_t count,
                                    size_t elemsize, pq_less_fun less);
priority_queue *pq_create_from_arr(const void *base, size_t count,
                                   size_t elemsize, pq_less_fun less);
void pq_destroy(priority_queue *pq);

void pq_push(priority_queue *pq, const void *elem);
void pq_pop(priority_queue *pq);
void pq_pushpop(priority_queue *pq, const void *elem);
bool pq_empty(priority_queue *pq);
const void *pq_top(priority_queue *pq);

size_t pq_size(priority_queue *pq);
size_t pq_esize(priority_queue *pq);
const void *pq_heap(priority_queue *pq);

void pq_heapify(void *base, size_t count, size_t elemsize, pq_less_fun less);

#define PQ_TOP(pq, type) (*(type *)pq_top(pq))

#define PQ_POP(pq, item)                                                       \
  do {                                                                         \
    item = PQ_TOP(pq, yu_typeof(item));                                        \
    pq_pop(pq);                                                                \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif // !YU_PRIORITY_QUEUE_H
