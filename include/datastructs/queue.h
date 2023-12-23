#ifndef YU_QUEUE_H
#define YU_QUEUE_H

#include "macros.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct queue queue;

/**
 * @brief Create Queue
 *
 * @param initial_capacity Initial capacity of the Queue
 * @param item_size Size of a single item in the Queue
 * @return Queue on success, 'NULL' otherwise
 */
queue *queue_create(size_t initial_capacity, size_t item_size);
/**
 * @brief Destroy Queue
 *
 * @param queue Queue
 */
void queue_destroy(queue *queue);
/**
 * @brief Push item into the Queue
 *
 * @param queue Queue
 * @param item Item to push
 * @return True on success, false on memory failure
 */
bool queue_push(queue *queue, const void *item);
/**
 * @brief Pop from the Queue
 *
 * @param queue Queue
 */
void queue_pop(queue *queue);
/**
 * @brief Front item of the Queue
 *
 * @param queue Queue
 */
void *queue_front(queue *queue);
/**
 * @brief Back item of the Queue
 *
 * @param queue Queue
 */
void *queue_back(queue *queue);
/**
 * @brief Checks if Queue is empty
 *
 * @param queue Queue
 * @return True if empty, false otherwise
 */
bool queue_empty(queue *queue);
/**
 * @brief Checks if Queue is full
 *
 * @param queue Queue
 * @return True if full, false otherwise
 */
bool queue_full(queue *queue);
/**
 * @brief Current Queue capacity
 *
 * @param queue Queue
 * @return Capacity of the queue
 */
size_t queue_capacity(queue *queue);
/**
 * @brief Number of items in the Queue
 *
 * @param queue Queue
 * @return Number of items in the Queue
 */
size_t queue_size(queue *queue);
/**
 * @brief Size of single item in the Queue
 *
 * @param queue Queue
 * @return Size of an item
 */
size_t queue_esize(queue *queue);

#define QUEUE_FRONT(queue, type) (*(type *)queue_front(queue))

#define QUEUE_BACK(queue, type) (*(type *)queue_back(queue))

#define QUEUE_POP(queue, item)                                                 \
  do {                                                                         \
    item = QUEUE_FRONT(Q, yu_typeof(item));                                    \
    queue_pop(queue);                                                          \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif // !YU_QUEUE_H
