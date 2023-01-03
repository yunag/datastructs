#ifndef YU_PRIORITY_QUEUE_H
#define YU_PRIORITY_QUEUE_H

#include "types.h"
#include <stdbool.h>

typedef struct priority_queue priority_queue;
typedef int (*cmp_fn)(const void *, const void *);

priority_queue *pq_create(size_t size, size_t elemsize, cmp_fn cmp);
void pq_free(priority_queue *pq);
void pq_push(priority_queue *pq, const void *elem);
size_t pq_size(priority_queue *pq);
bool pq_empty(priority_queue *pq);
const void *pq_top(priority_queue *pq);
void pq_pop(priority_queue *pq);

#endif // !YU_PRIORITY_QUEUE_H
