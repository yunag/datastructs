#ifndef YU_PRIORITY_QUEUE_H
#define YU_PRIORITY_QUEUE_H

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct priority_queue priority_queue;
typedef int (*cmp_fn)(const void *, const void *);

priority_queue *pq_create(size_t size, size_t elemsize, cmp_fn cmp);
void pq_free(priority_queue *pq);
void pq_push(priority_queue *pq, const void *elem);
void pq_pop(priority_queue *pq);
bool pq_empty(priority_queue *pq);
const void *pq_top(priority_queue *pq);
size_t pq_size(priority_queue *pq);
size_t pq_esize(priority_queue *pq);

#define PQ_PUSH(PQ, elem)                                                      \
  do {                                                                         \
    __typeof__(elem) __elem = (elem);                                          \
    pq_push(PQ, &__elem);                                                      \
  } while (0)
#define PQ_TOP(PQ, T) (*(T *)pq_top(PQ))
#define PQ_POP(PQ, ret)                                                        \
  do {                                                                         \
    ret = PQ_TOP(PQ, __typeof__(ret));                                         \
    pq_pop(PQ);                                                                \
  } while (0)
#ifdef __cplusplus
}
#endif

#endif // !YU_PRIORITY_QUEUE_H
