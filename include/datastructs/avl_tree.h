#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <datastructs/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct avl_tree avl_tree;
typedef struct avl_iterator avl_iterator;

avl_tree *avl_create(cmp_fn cmp_key, free_fn free_key, free_fn free_val);
void avl_destroy(avl_tree *avl);
void avl_insert(avl_tree *avl, void *key, void *val);
bool avl_find(avl_tree *avl, const void *key);
void avl_remove(avl_tree *avl, const void *key);
size_t avl_size(avl_tree *avl);
bool avl_valid_avl(avl_tree *avl);

avl_iterator *avl_first(avl_tree *avl);
void avl_next(avl_iterator *iterator);
bool avl_has_next(avl_iterator *iterator);
struct key_value avl_get(avl_iterator *iterator);
void avl_it_destroy(avl_iterator *iterator);

#define AVL_FOR_EACH(it, KeyT, ValT, keyname, valname)                         \
  for (; avl_has_next(it); avl_next(it))                                       \
    for (bool _should_loop = true; _should_loop;)                              \
      for (KeyT keyname = (KeyT)avl_get(it).key; _should_loop;)                \
        for (ValT valname = (ValT)avl_get(it).val; _should_loop;               \
             _should_loop = false)

#define AVL_FOR_EACH_IT(it) for (; avl_has_next(it); avl_next(it))

#ifdef __cplusplus
}
#endif

#endif /* BINARY_SEARCH_TREE_H */
