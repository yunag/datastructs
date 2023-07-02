#ifndef YU_PRIORITY_QUEUE_H
#define YU_PRIORITY_QUEUE_H

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct priority_queue priority_queue;

priority_queue *pq_create(size_t capacity, size_t elemsize, compare_fn cmp);
void pq_destroy(priority_queue *pq);
void pq_push(priority_queue *pq, const void *elem);
void pq_pop(priority_queue *pq);
bool pq_empty(priority_queue *pq);
const void *pq_top(priority_queue *pq);
size_t pq_size(priority_queue *pq);
size_t pq_esize(priority_queue *pq);
const void *pq_heap(priority_queue *pq);
priority_queue *pq_create_from_heap(const void *heap, size_t count,
                                    size_t elemsize, compare_fn cmp);
priority_queue *pq_create_from_arr(const void *base, size_t count,
                                   size_t elemsize, compare_fn cmp);

void heapify(void *base, size_t count, size_t elemsize, compare_fn cmp);

#define PQ_PUSH(priority_queue, elem)                                          \
  do {                                                                         \
    __typeof__(elem) __elem = (elem);                                          \
    pq_push(priority_queue, &__elem);                                          \
  } while (0)
#define PQ_TOP(priority_queue, type) (*(type *)pq_top(priority_queue))
#define PQ_POP(priority_queue, return_value)                                   \
  do {                                                                         \
    return_value = PQ_TOP(priority_queue, __typeof__(return_value));           \
    pq_pop(priority_queue);                                                    \
  } while (0)
#ifdef __cplusplus
}
#endif

#endif // !YU_PRIORITY_QUEUE_H
