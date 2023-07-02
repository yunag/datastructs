#ifndef YU_MEMORY_H
#define YU_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *yu_allocate(size_t size);
void *yu_calloc(size_t count, size_t size);
void *yu_realloc(void *block, size_t size);
void yu_free(void *block);

#ifdef __cplusplus
}
#endif

#endif /* !YU_MEMORY_H */
