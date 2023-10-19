#ifndef YU_MEMORY_H
#define YU_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct yu_allocator {
  void *(*allocate)(size_t);
  void *(*calloc)(size_t, size_t);
  void *(*realloc)(void *, size_t);
  void (*free)(void *);
};

extern struct yu_allocator _yu_allocator;

#ifdef __cplusplus
}
#endif

#endif /* !YU_MEMORY_H */
