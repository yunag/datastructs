#ifndef YU_QUEUE_H
#define YU_QUEUE_H

#include "types.h"
#include <stdbool.h>

typedef struct queue queue;

queue *queue_create(size_t size, size_t elemsize);
void queue_free(queue *queue);
void queue_push(queue *queue, void *elem);
void queue_pop(queue *queue);
void *queue_front(queue *queue);
void *queue_back(queue *queue);
bool queue_empty(queue *queue);
bool queue_full(queue *queue);

#endif // !YU_QUEUE_H
