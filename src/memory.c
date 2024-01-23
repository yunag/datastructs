#include "datastructs/memory.h"
#include "datastructs/macros.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static struct yu_allocator g_allocator = {
  .allocate = yu_default_allocate,
  .reallocate = yu_default_reallocate,
  .deallocate = yu_default_deallocate,
  .user_data = NULL,
};

void *yu_default_allocate(size_t size, void *user_data) {
  YU_UNUSED(user_data);
  return malloc(size);
}

void *yu_default_reallocate(void *block, size_t size, void *user_data) {
  YU_UNUSED(user_data);
  return realloc(block, size);
}

void yu_default_deallocate(void *block, void *user_data) {
  YU_UNUSED(user_data);
  free(block);
}

void yu_set_allocator(const yu_allocator *allocator) {
  assert(allocator->allocate != NULL);
  assert(allocator->reallocate != NULL);
  assert(allocator->deallocate != NULL);

  g_allocator = *allocator;
}

void *yu_malloc(size_t size) {
  return g_allocator.allocate(size, g_allocator.user_data);
}

void *yu_realloc(void *block, size_t size) {
  return g_allocator.reallocate(block, size, g_allocator.user_data);
}

void *yu_calloc(size_t count, size_t size) {
  if (count > SIZE_MAX / size) {
    /* count multiplied by size will overflow */
    return NULL;
  }

  size_t block_size = count * size;

  void *block = g_allocator.allocate(block_size, g_allocator.user_data);
  if (block) {
    return memset(block, 0, block_size);
  }

  return NULL;
}

void yu_free(void *block) {
  g_allocator.deallocate(block, g_allocator.user_data);
}
