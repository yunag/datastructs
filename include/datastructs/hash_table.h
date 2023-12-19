#ifndef YU_HASH_TABLE_H
#define YU_HASH_TABLE_H

#include "datastructs/macros.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ht_entry(ptr, type, member) YU_CONTAINER_OF(ptr, type, member)

#define ht_entry_safe(ptr, type, member) YU_CONTAINER_OF_SAFE(ptr, type, member)

typedef struct hash_table hash_table;

struct hash_entry {
  /* List of all entries */
  struct hash_entry *ht_next;
  struct hash_entry *ht_prev;

  struct hash_entry *next; /* Pointer to next entry in current bucket */

  size_t hashv; /* Result of hash function */
};

struct hash_bucket {
  struct hash_entry *entry;
};

typedef void (*ht_destroy_fun)(hash_table *);
typedef bool (*ht_equal_fun)(const struct hash_entry *,
                             const struct hash_entry *);
typedef bool (*ht_less_fun)(const struct hash_entry *,
                            const struct hash_entry *);
typedef size_t (*ht_hash_fun)(const struct hash_entry *);

hash_table *htable_create(size_t initial_num_buckets, ht_hash_fun hash,
                          ht_equal_fun equal);
void htable_destroy(hash_table *htable, ht_destroy_fun destroy);
bool htable_rehash(hash_table *htable, size_t newsize);
bool htable_insert(hash_table *htable, struct hash_entry *entry);
bool htable_replace(hash_table *htable, struct hash_entry *entry,
                    struct hash_entry **replaced);
struct hash_entry *htable_lookup(hash_table *htable, struct hash_entry *query);
struct hash_entry *htable_remove(hash_table *htable, struct hash_entry *query);
bool htable_erase(hash_table *htable, struct hash_entry *entry);
void htable_sort(hash_table *htable, ht_less_fun less);
size_t htable_size(hash_table *htable);

struct hash_entry *htable_last(hash_table *htable);
struct hash_entry *htable_first(hash_table *htable);
struct hash_entry *htable_next(const struct hash_entry *entry);
struct hash_entry *htable_prev(const struct hash_entry *entry);

#define ht_find(htable, query, field)                                          \
  ht_entry_safe(htable_lookup(htable, &(query)->field), yu_typeof(*query),     \
                field)

#define ht_remove(htable, query, field)                                        \
  ht_entry_safe(htable_remove(htable, &(query)->field), yu_typeof(*query),     \
                field)

#define ht_for_each(htable, cur, field)                                        \
  for (cur = ht_entry_safe(htable_first(htable), yu_typeof(*cur), field); cur; \
       cur = ht_entry_safe(htable_next(&cur->field), yu_typeof(*cur), field))

#define ht_for_each_temp(htable, cur, n, field)                                \
  for (cur = ht_entry_safe(htable_first(htable), yu_typeof(*cur), field);      \
       cur && ((n = ht_entry_safe(htable_next(&cur->field), yu_typeof(*cur),   \
                                  field)) ||                                   \
               1);                                                             \
       cur = n)

#ifdef __cplusplus
}
#endif

#endif // !YU_HASH_TABLE_H
