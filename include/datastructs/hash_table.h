#ifndef YU_HASH_TABLE_H
#define YU_HASH_TABLE_H

#include <datastructs/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ht_entry(ptr, type, member) YU_CONTAINER_OF(ptr, type, member)

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

typedef int (*compare_ht_fun)(const struct hash_entry *,
                              const struct hash_entry *);
typedef struct hash_entry **(*lookup_ht_fun)(hash_table *,
                                             const struct hash_entry *,
                                             struct hash_bucket *);
typedef uint64_t (*hash_entry_fun)(const struct hash_entry *);
typedef void (*destroy_ht_fun)(struct hash_entry *);
hash_table *htable_create(size_t initial_capacity, hash_entry_fun hash,
                          compare_ht_fun cmp, destroy_ht_fun destroy);
void htable_destroy(hash_table *htable);
bool htable_rehash(hash_table *htable, size_t newsize);
bool htable_insert(hash_table *htable, struct hash_entry *hentry);
struct hash_entry *htable_lookup(hash_table *htable, struct hash_entry *query);
bool htable_remove(hash_table *htable, struct hash_entry *query);
bool htable_delete(hash_table *htable, struct hash_entry *hentry);
void htable_sort(hash_table *htable, compare_ht_fun cmp);
size_t htable_size(hash_table *htable);

struct hash_entry *htable_last(hash_table *htable);
struct hash_entry *htable_first(hash_table *htable);
struct hash_entry *htable_next(const struct hash_entry *entry);
struct hash_entry *htable_prev(const struct hash_entry *entry);

#define HTABLE_FOR_EACH(htable, entry)                                         \
  for (hash_entry *entry = htable_first(htable); entry;                        \
       entry = htable_next(entry))

#ifdef __cplusplus
}
#endif

#endif // !YU_HASH_TABLE_H
