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

typedef void (*destroy_table_fun)(hash_table *ht);
typedef bool (*equal_ht_fun)(const struct hash_entry *,
                             const struct hash_entry *);
typedef bool (*less_ht_fun)(const struct hash_entry *,
                            const struct hash_entry *);
typedef struct hash_entry **(*lookup_ht_fun)(const struct hash_entry *,
                                             struct hash_bucket *);
typedef size_t (*hash_entry_fun)(const struct hash_entry *);

hash_table *htable_create(size_t initial_num_buckets, hash_entry_fun hash,
                          equal_ht_fun equal);
void htable_destroy(hash_table *htable, destroy_table_fun destroy);
bool htable_rehash(hash_table *htable, size_t newsize);
bool htable_insert(hash_table *htable, struct hash_entry *hentry);
bool htable_replace(hash_table *htable, struct hash_entry *hentry,
                    struct hash_entry **replaced);
struct hash_entry *htable_lookup(hash_table *htable, struct hash_entry *query);
struct hash_entry *htable_remove(hash_table *htable, struct hash_entry *query);
bool htable_erase(hash_table *htable, struct hash_entry *hentry);
void htable_sort(hash_table *htable, less_ht_fun less);
size_t htable_size(hash_table *htable);

struct hash_entry *htable_last(hash_table *htable);
struct hash_entry *htable_first(hash_table *htable);
struct hash_entry *htable_next(const struct hash_entry *entry);
struct hash_entry *htable_prev(const struct hash_entry *entry);

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
