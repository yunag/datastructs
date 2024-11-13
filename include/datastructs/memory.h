#ifndef YU_MEMORY_H
#define YU_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct yu_allocator {
  void *(*allocate)(size_t, void *);
  void *(*reallocate)(void *, size_t, void *);
  void (*deallocate)(void *, void *);

  void *user_data;
} yu_allocator;

void yu_set_allocator(const yu_allocator *allocator);

void *yu_malloc(size_t size);
void *yu_realloc(void *block, size_t size);
void *yu_calloc(size_t count, size_t size);
void yu_free(void *block);

void *yu_default_allocate(size_t size, void *user_data);
void *yu_default_reallocate(void *block, size_t size, void *user_data);
void yu_default_deallocate(void *block, void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* !YU_MEMORY_H */
