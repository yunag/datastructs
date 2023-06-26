#ifndef YU_HASH_TABLE_H
#define YU_HASH_TABLE_H

#include <datastructs/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash_table hash_table;

typedef struct hash_entry {
  void *key;
  void *val;

  /* Linked list of all entries */
  struct hash_entry *ll_next;
  struct hash_entry *ll_prev;

  struct hash_entry *next; /* Pointer to next entry in current bucket */
} hash_entry;

typedef uint64_t (*hash_fn)(const void *);

hash_table *htable_create(size_t initial_capacity, hash_fn hash, cmp_fn cmp_key,
                          free_fn free_key, free_fn free_value);
void htable_destroy(hash_table *htable);
bool htable_insert(hash_table *htable, void *key, void *val);
void *htable_lookup(hash_table *htable, const void *key);
bool htable_contains(hash_table *htable, const void *key);
bool htable_remove(hash_table *htable, const void *key);
size_t htable_size(hash_table *htable);

hash_entry *ht_last(hash_table *htable);
hash_entry *ht_first(hash_table *htable);
hash_entry *ht_next(hash_entry *iterator);
hash_entry *ht_prev(hash_entry *iterator);

#define HT_FOR_EACH(htable, KeyT, ValT, keyname, valname)                      \
  for (bool _should_loop = true, _should_break = false; _should_loop;          \
       _should_loop = false)                                                   \
    for (hash_entry *_it = ht_first(htable); _it && !_should_break;            \
         _it = _should_break ? _it : ht_next(_it), _should_loop = true)        \
      for (KeyT keyname = (KeyT)_it->key; _should_loop;                        \
           _should_break = _should_loop, _should_loop = false)                 \
        for (ValT valname = (ValT)_it->val; _should_loop; _should_loop = false)

#define HT_FOR_EACH_IT(htable, iterator_name)                                  \
  for (hash_entry *iterator_name = ht_first(htable); iterator_name;            \
       iterator_name = ht_next(iterator_name))

#define HT_FIND(htable, key, ValT) ((ValT)htable_lookup(htable, (key)))

#ifdef __cplusplus
}
#endif

#endif // !YU_HASH_TABLE_H
