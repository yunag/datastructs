#ifndef YU_HASH_TABLE_H
#define YU_HASH_TABLE_H

#include <datastructs/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hash_table hash_table;
typedef struct ht_iterator ht_iterator;

typedef uint64_t (*hash_fn)(const void *);

hash_table *htable_create(size_t initial_capacity, hash_fn hash, cmp_fn cmp_key,
                          free_fn free_key, free_fn free_value);
void htable_destroy(hash_table *htable);
bool htable_insert(hash_table *htable, void *key, void *val);
void *htable_lookup(hash_table *htable, const void *key);
bool htable_contains(hash_table *htable, const void *key);
bool htable_remove(hash_table *htable, const void *key);
size_t htable_size(hash_table *htable);

ht_iterator *ht_begin(hash_table *htable);
ht_iterator *ht_end(hash_table *htable);
ht_iterator *ht_next(ht_iterator *iterator);
ht_iterator *ht_prev(ht_iterator *iterator);
struct key_value ht_get(ht_iterator *iterator);

#define HT_FOR_EACH(htable, KeyT, ValT, keyname, valname)                      \
  for (ht_iterator *_it = ht_begin((htable)); _it != ht_end((htable));         \
       _it = ht_next(_it))                                                     \
    for (bool _should_loop = true; _should_loop;)                              \
      for (KeyT keyname = (KeyT)ht_get(_it).key; _should_loop;)                \
        for (ValT valname = (ValT)ht_get(_it).val; _should_loop;               \
             _should_loop = false)

#define HT_FOR_EACH_IT(htable, iterator_name)                                  \
  for (ht_iterator *iterator_name = ht_begin((htable));                        \
       iterator_name != ht_end((htable));                                      \
       iterator_name = ht_next(iterator_name))

#define HT_FIND(htable, key, ValT) ((ValT)htable_lookup(htable, (key)))

#ifdef __cplusplus
}
#endif

#endif // !YU_HASH_TABLE_H
