#ifndef YU_HASH_TABLE_H
#define YU_HASH_TABLE_H

#include "types.h"

#include <stdbool.h>
typedef struct hash_table hash_table;

hash_table *hash_table_create(size_t table_size, size_t key_size,
                              size_t value_size);

void hash_table_free(hash_table *hash_table);

bool hash_table_insert(hash_table *hash_table, void *key, void *val);

void *hash_table_lookup(hash_table *hash_table, void *key);

#endif // !YU_HASH_TABLE_H
