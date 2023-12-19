#ifndef YU_QUEUE_H
#define YU_QUEUE_H

#include "macros.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct queue queue;

queue *queue_create(size_t initial_capacity, size_t elemsize);
void queue_destroy(queue *queue);
void queue_push(queue *queue, const void *elem);
void queue_pop(queue *queue);
void *queue_front(queue *queue);
void *queue_back(queue *queue);
bool queue_empty(queue *queue);
bool queue_full(queue *queue);
size_t queue_size(queue *queue);
size_t queue_esize(queue *queue);

#define QUEUE_PUSH(queue, elem)                                                \
  do {                                                                         \
    yu_typeof(elem) __elem = (elem);                                           \
    queue_push(queue, &__elem);                                                \
  } while (0)
#define QUEUE_FRONT(queue, type) (*(type *)queue_front(queue))
#define QUEUE_BACK(queue, type) (*(type *)queue_back(queue))
#define QUEUE_POP(queue, return_value)                                         \
  do {                                                                         \
    return_value = QUEUE_FRONT(Q, yu_typeof(return_value));                    \
    queue_pop(queue);                                                          \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif // !YU_QUEUE_H
