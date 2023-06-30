#ifndef YU_HASH_TABLE_H
#define YU_HASH_TABLE_H

#include <datastructs/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash_table hash_table;
typedef struct hash_entry hash_entry;

typedef int (*cmp_ht_entries_fn)(const struct hash_entry *,
                                 const struct hash_entry *);
typedef uint64_t (*hash_ht_entries_fn)(const struct hash_entry *);
typedef void (*destroy_ht_entry_fn)(struct hash_entry *);

#define ht_entry(ptr, type, member) YU_CONTAINER_OF(ptr, type, member)

struct hash_entry {
  /* Linked list of all entries */
  struct hash_entry *ht_next;
  struct hash_entry *ht_prev;

  struct hash_entry *next; /* Pointer to next entry in current bucket */
};

hash_table *htable_create(size_t initial_capacity, hash_ht_entries_fn hash,
                          cmp_ht_entries_fn cmp, destroy_ht_entry_fn destroy);
void htable_destroy(hash_table *htable);
bool htable_insert(hash_table *htable, hash_entry *ht_entry);
hash_entry *htable_lookup(hash_table *htable, const hash_entry *query);
bool htable_remove(hash_table *htable, const hash_entry *query);
size_t htable_size(hash_table *htable);

hash_entry *ht_last(hash_table *htable);
hash_entry *ht_first(hash_table *htable);
hash_entry *ht_next(hash_entry *entry);
hash_entry *ht_prev(hash_entry *entry);

#define HT_FOR_EACH(htable, entry)                                             \
  for (hash_entry *entry = ht_first(htable); entry; entry = ht_next(entry))

#ifdef __cplusplus
}
#endif

#endif // !YU_HASH_TABLE_H
