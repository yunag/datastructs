#ifndef YU_HASH_TABLE_H
#define YU_HASH_TABLE_H

#include "macros.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define htable_entry(ptr, type, member) YU_CONTAINER_OF(ptr, type, member)

#define htable_entry_safe(ptr, type, member)                                   \
  YU_CONTAINER_OF_SAFE(ptr, type, member)

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

/**
 * @brief Create Hash Table
 *
 * @param initial_num_buckets Initial number of buckets
 * @param hash Function to hash your entry
 * @param equal Function to compare two entries
 * @return Hash Table on success, 'NULL otherwise
 */
hash_table *htable_create(size_t initial_num_buckets, ht_hash_fun hash,
                          ht_equal_fun equal);

/**
 * @brief Destroy Hash Table
 *
 * @param htable Hash Table
 * @param destroy Function to destroy your entries, could be 'NULL'
 */
void htable_destroy(hash_table *htable, ht_destroy_fun destroy);

/**
 * @brief Rehash Hash Table
 *
 * @param htable Hash Table
 * @param new_num_buckets New number of buckets
 * @return True on success, false on memory failure
 */
bool htable_rehash(hash_table *htable, size_t new_num_buckets);

/**
 * @brief Insert entry into Hash Table
 *
 * @param htable Hash Table
 * @param entry Entry to insert
 * @return True on success, false on memory failure
 */
bool htable_insert(hash_table *htable, struct hash_entry *entry);

/**
 * @brief Replace entry in the Hash Table
 *
 * @param htable Hash Table
 * @param entry Entry to replace with
 * @param replaced Replaced entry
 * @return True on success, false on memory failure
 */
bool htable_replace(hash_table *htable, struct hash_entry *entry,
                    struct hash_entry **replaced);

/**
 * @brief Lookup entry in the Hash Table
 *
 * @param htable Hash Table
 * @param query Query to lookup against
 */
struct hash_entry *htable_lookup(hash_table *htable, struct hash_entry *query);

/**
 * @brief Lookup entry and remove it from the Hash Table
 *
 * @param htable Hash Table
 * @param query Query to lookup against
 */
struct hash_entry *htable_remove(hash_table *htable, struct hash_entry *query);

/**
 * @brief Remove entry
 *
 * Use this function when you want to remove
 * entry without lookup
 *
 * @param htable Hash Table
 * @param entry Entry to remove
 */
void htable_erase(hash_table *htable, struct hash_entry *entry);

/**
 * @brief Sort table
 *
 * Time Complexity: O(n * log(n))
 * Space Complexity: O(1)
 *
 * After sorting the table you will be able to iterate over it
 * in ascending(or descending) order.
 *
 * @param htable Hash Table
 * @param less Function to compare two entries
 */
void htable_sort(hash_table *htable, ht_less_fun less);

/**
 * @brief Number of entries in the Hash Table
 *
 * @param htable Hash Table
 * @return Number of entries
 */
size_t htable_size(hash_table *htable);

/**
 * @brief Number of buckets in the Hash Table
 *
 * @param htable Hash Table
 * @return Number of buckets
 */
size_t htable_num_buckets(hash_table *htable);

/**
 * @brief First entry in the Hash Table
 *
 * @param htable Hash Table
 */
struct hash_entry *htable_first(hash_table *htable);

/**
 * @brief Last entry in the Hash Table
 *
 * @param htable Hash Table
 */
struct hash_entry *htable_last(hash_table *htable);

/**
 * @brief Next entry in the Hash Table
 *
 * @param htable Hash Table
 */
struct hash_entry *htable_next(const struct hash_entry *entry);

/**
 * @brief Previous entry in the Hash Table
 *
 * @param htable Hash Table
 */
struct hash_entry *htable_prev(const struct hash_entry *entry);

#define htable_find(htable, query, field)                                      \
  htable_entry_safe(htable_lookup(htable, &(query)->field), yu_typeof(*query), \
                    field)

#define htable_delete(htable, query, field)                                    \
  htable_entry_safe(htable_remove(htable, &(query)->field), yu_typeof(*query), \
                    field)

#define htable_add(htable, entry, field) htable_insert(htable, &(entry)->field)

#define htable_for_each(htable, cur, field)                                    \
  for (cur = htable_entry_safe(htable_first(htable), yu_typeof(*cur), field);  \
       cur; cur = htable_entry_safe(htable_next(&cur->field), yu_typeof(*cur), \
                                    field))

#define htable_for_each_temp(htable, cur, n, field)                            \
  for (cur = htable_entry_safe(htable_first(htable), yu_typeof(*cur), field);  \
       cur && ((n = htable_entry_safe(htable_next(&cur->field),                \
                                      yu_typeof(*cur), field)) ||              \
               1);                                                             \
       cur = n)

#ifdef __cplusplus
}
#endif

#endif  // !YU_HASH_TABLE_H
