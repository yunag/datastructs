#ifndef YU_MEMORY_H
#define YU_MEMORY_H

void *yu_allocate(size_t size);
void *yu_calloc(size_t count, size_t size);
void *yu_realloc(void *block, size_t size);
void yu_free(void *block);

#endif /* !YU_MEMORY_H */
