#ifndef YU_PRIORITY_QUEUE_H
#define YU_PRIORITY_QUEUE_H

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct priority_queue priority_queue;
typedef int (*cmp_fn)(const void *, const void *);

#define PQ_PUSH(PQ, T, elem)                                                   \
  do {                                                                         \
    assert(sizeof(T) == pq_esize(PQ));                                         \
    T __elem = (elem);                                                         \
    pq_push(PQ, &__elem);                                                      \
  } while (0)

#define PQ_TOP(PQ, T) *(T *)pq_top(PQ)

priority_queue *pq_create(size_t size, size_t elemsize, cmp_fn cmp);
void pq_free(priority_queue *pq);
void pq_push(priority_queue *pq, const void *elem);
void pq_pop(priority_queue *pq);
bool pq_empty(priority_queue *pq);
const void *pq_top(priority_queue *pq);
size_t pq_size(priority_queue *pq);
size_t pq_esize(priority_queue *pq);

#ifdef __cplusplus
}
#endif

#endif // !YU_PRIORITY_QUEUE_H
