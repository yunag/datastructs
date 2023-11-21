#ifndef YU_HASH_TABLE_H
#define YU_HASH_TABLE_H

#include <datastructs/macros.h>
#include <datastructs/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ht_entry(ptr, type, member) YU_CONTAINER_OF(ptr, type, member)

#define ht_entry_safe(ptr, type, member)                                       \
  ({                                                                           \
    __typeof__(ptr) _ptr = (ptr);                                              \
    _ptr ? ht_entry(_ptr, type, member) : NULL;                                \
  })

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

hash_table *htable_create(size_t initial_capacity, hash_entry_fun hash,
                          equal_ht_fun equal);
void htable_destroy(hash_table *htable, destroy_table_fun destroy);
bool htable_rehash(hash_table *htable, size_t newsize);
bool htable_insert(hash_table *htable, struct hash_entry *hentry);
bool htable_replace(hash_table *htable, struct hash_entry *hentry,
                    struct hash_entry **replaced);
struct hash_entry *htable_lookup(hash_table *htable, struct hash_entry *query);
struct hash_entry *htable_remove(hash_table *htable, struct hash_entry *query);
bool htable_delete(hash_table *htable, struct hash_entry *hentry);
void htable_sort(hash_table *htable, less_ht_fun less);
size_t htable_size(hash_table *htable);

struct hash_entry *htable_last(hash_table *htable);
struct hash_entry *htable_first(hash_table *htable);
struct hash_entry *htable_next(const struct hash_entry *entry);
struct hash_entry *htable_prev(const struct hash_entry *entry);

#define HTABLE_FOR_EACH(htable, cur, field)                                    \
  for (cur = ht_entry_safe(htable_first(htable), __typeof__(*cur), field);     \
       cur;                                                                    \
       cur = ht_entry_safe(htable_next(&cur->field), __typeof__(*cur), field))

#define HTABLE_FOR_EACH_TEMP(htable, cur, n, field)                            \
  for (cur = ht_entry_safe(htable_first(htable), __typeof__(*cur), field);     \
       cur && ({                                                               \
         n = ht_entry_safe(htable_next(&cur->field), __typeof__(*cur), field); \
         1;                                                                    \
       });                                                                     \
       cur = n)

#define HTABLE_FOR_EACH_ENTRY(htable, entry)                                   \
  for (struct hash_entry *entry = htable_first(htable); entry;                 \
       entry = htable_next(entry))

#ifdef __cplusplus
}
#endif

#endif // !YU_HASH_TABLE_H
