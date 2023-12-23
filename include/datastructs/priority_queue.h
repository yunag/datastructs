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

/**
 * @brief Create Priority Queue
 *
 * @param initial_capacity Initial capacity
 * @param item_size Size of a single item
 * @param less Function to compare two items
 * @return Priority Queue on success, 'NULL' otherwise
 */
priority_queue *pq_create(size_t initial_capacity, size_t item_size,
                          pq_less_fun less);
/**
 * @brief Create Priority Queue from heap
 *
 * @param heap Heap
 * @param count Number of items in the heap
 * @param item_size Size of single item in the heap
 * @param less Function to compare two items
 * @return Priority Queue on success, 'NULL' otherwise
 */
priority_queue *pq_create_from_heap(const void *heap, size_t count,
                                    size_t item_size, pq_less_fun less);

/**
 * @brief Create Priority Queue from array
 *
 * @param base Array
 * @param count Number of items in the array
 * @param item_size Size of single item in the array
 * @param less Function to compare two items
 * @return Priority Queue on success, 'NULL' otherwise
 */
priority_queue *pq_create_from_arr(const void *base, size_t count,
                                   size_t item_size, pq_less_fun less);
/**
 * @brief Destroy Priority Queue
 *
 * @param pq Priority Queue
 */
void pq_destroy(priority_queue *pq);

/**
 * @brief Push item into the Priority Queue
 *
 * @param pq Priority Queue
 * @param item Item to push
 */
void pq_push(priority_queue *pq, const void *item);
/**
 * @brief Pop item from Priority Queue
 *
 * @param pq Priority Queue
 * @return Nothing
 */
void pq_pop(priority_queue *pq);
/**
 * @brief Push and pop at the same time
 *
 * Use this function when you want to POP item from a
 * Priority Queue and then PUSH an item into Priority Queue.
 *
 * @param pq Priority Queue
 * @param item Item to push
 */
void pq_pushpop(priority_queue *pq, const void *item);
/**
 * @brief Checks if Priority Queue is empty
 *
 * @param pq Priority Queue
 * @return True if empty, false otherwise
 */
bool pq_empty(priority_queue *pq);
/**
 * @brief Top item of Priority Queue
 *
 * @param pq Priority Queue
 */
const void *pq_top(priority_queue *pq);

/**
 * @brief Number of items in the Priority Queue
 *
 * @param pq Priority Queue
 * @return Number of items in Priority Queue
 */
size_t pq_size(priority_queue *pq);
/**
 * @brief Size of a single item in the Priority Queue
 *
 * @param pq Priority Queue
 * @return Size of a single item
 */
size_t pq_esize(priority_queue *pq);
/**
 * @brief Underline heap
 *
 * @param pq Priority Queue
 */
const void *pq_heap(priority_queue *pq);

/**
 * @brief Heapify an array
 *
 * @param base Array
 * @param count Number of items in the array
 * @param item_size Size of a single item in the array
 * @param less Function to compare two items
 */
void pq_heapify(void *base, size_t count, size_t item_size, pq_less_fun less);

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
