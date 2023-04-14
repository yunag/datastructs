#ifndef YU_QUEUE_H
#define YU_QUEUE_H

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct queue queue;

queue *queue_create(size_t capacity, size_t elemsize, free_fn free_elem);
void queue_free(queue *queue);
void queue_push(queue *queue, const void *elem);
void queue_pop(queue *queue);
void *queue_front(queue *queue);
void *queue_back(queue *queue);
bool queue_empty(queue *queue);
bool queue_full(queue *queue);
size_t queue_size(queue *queue);
size_t queue_esize(queue *queue);

#define QUEUE_PUSH(Q, elem)                                                    \
  do {                                                                         \
    __typeof__(elem) __elem = (elem);                                          \
    queue_push(Q, &__elem);                                                    \
  } while (0)

#define QUEUE_FRONT(Q, T) (*(T *)queue_front(Q))
#define QUEUE_BACK(Q, T) (*(T *)queue_back(Q))
#define QUEUE_POP(Q, ret)                                                      \
  do {                                                                         \
    ret = QUEUE_FRONT(Q, __typeof__(ret));                                     \
    queue_pop(Q);                                                              \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif // !YU_QUEUE_H
