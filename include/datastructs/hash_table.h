#ifndef YU_HASH_TABLE_H
#define YU_HASH_TABLE_H

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash_table hash_table;

hash_table *htable_create(size_t table_size, size_t key_size,
                          size_t value_size);
void htable_free(hash_table *hash_table);
void htable_insert(hash_table *hash_table, const void *key, const void *val);
void *htable_lookup(hash_table *hash_table, const void *key);
void htable_remove(hash_table *htable, const void *key);
size_t htable_size(hash_table *hash_table);
size_t htable_ksize(hash_table *hash_table);
size_t htable_vsize(hash_table *hash_table);

#define HASH_INSERT(HT, key, val)                                              \
  do {                                                                         \
    __typeof__(key) __key = (key);                                             \
    __typeof__(val) __val = (val);                                             \
    htable_insert(HT, &__key, &__val);                                         \
  } while (0)

#define HASH_FIND(HT, key, ret)                                                \
  do {                                                                         \
    __typeof__ __key = (key);                                                  \
    ret = htable_lookup(HT, &__key);                                           \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif // !YU_HASH_TABLE_H
