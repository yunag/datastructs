#include "datastructs/memory.h"
#include "datastructs/macros.h"

#include <stdlib.h>
#include <string.h>

void *yu_allocate(size_t size) {
  void *block = malloc(size);
  if (!block) {
    YU_LOG_ERROR("Out of memory");
  }
  return block;
}

void *yu_calloc(size_t count, size_t size) {
  if (!count || !size) {
    return NULL;
  }
  void *block;
  if (count > SIZE_MAX / size) {
    YU_LOG_ERROR("Allocation size overflow");
    return NULL;
  }
  block = malloc(count * size);
  if (!block) {
    YU_LOG_ERROR("Out of memory");
    return NULL;
  }
  return memset(block, 0, count * size);
}

void *yu_realloc(void *block, size_t size) {
  if (block && size) {
    void *resized = realloc(block, size);
    if (!resized) {
      YU_LOG_ERROR("Out of memory");
    }
    return resized;
  } else if (block) {
    free(block);
    return NULL;
  } else {
    return yu_calloc(1, size);
  }
}

void yu_free(void *block) { free(block); }
