#ifndef YU_HASH_TABLE_H
#define YU_HASH_TABLE_H

#include "types.h"
#include <stdbool.h>

typedef struct hash_table hash_table;

#define HASH_INSERT(HT, keyT, valT, key, val)                                  \
  do {                                                                         \
    assert(sizeof(keyT) == htable_ksize((HT)) &&                                 \
           sizeof(valT) == htable_vsize((HT));                                  \
    keyT keycopy = (key);                                                      \
    valT valcopy = (val);                                                      \
    htable_insert((HT), &keycopy, &valcopy);                                   \
  } while (0)

#define HASH_FIND(HT, keyT, key)                                               \
  ({                                                                           \
    assert(sizeof(keyT) == htable_ksize((HT)));                                \
    keyT keycopy = (key);                                                      \
    htable_lookup((HT), &keycopy);                                             \
  })

hash_table *htable_create(size_t table_size, size_t key_size,
                          size_t value_size);
void htable_free(hash_table *hash_table);
void htable_insert(hash_table *hash_table, const void *key, const void *val);
void *htable_lookup(hash_table *hash_table, const void *key);
inline size_t htable_size(hash_table *hash_table);
inline size_t htable_ksize(hash_table *hash_table);
inline size_t htable_vsize(hash_table *hash_table);

#endif // !YU_HASH_TABLE_H
