#ifndef YU_PRIORITY_QUEUE_H
#define YU_PRIORITY_QUEUE_H

#include "types.h"
#include <stdbool.h>

typedef struct priority_queue priority_queue;
typedef int (*cmp_fn)(const void *, const void *);

#define PQ_PUSH(PQ, elemT, elem)                                               \
  do {                                                                         \
    assert(sizeof(elemT) == pq_esize((PQ)));                                   \
    elemT elemcopy = (elem);                                                   \
    pq_push((PQ), &elemcopy);                                                  \
  } while (0)

#define PQ_TOP(Q, elemT) *(elemT *)pq_top((Q))

priority_queue *pq_create(size_t size, size_t elemsize, cmp_fn cmp);
void pq_free(priority_queue *pq);
void pq_push(priority_queue *pq, const void *elem);
void pq_pop(priority_queue *pq);
inline size_t pq_size(priority_queue *pq);
inline bool pq_empty(priority_queue *pq);
inline const void *pq_top(priority_queue *pq);
inline size_t pq_esize(priority_queue *pq);

#endif // !YU_PRIORITY_QUEUE_H
