#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <datastructs/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct avl_tree avl_tree;
typedef struct avl_node avl_node;

struct avl_node {
  struct avl_node *left;
  struct avl_node *right;
  struct avl_node *parent;

  void *key;
  void *val;
  size_t height;
};

struct kv_node {
  struct avl_node *left;
  struct avl_node *right;
  struct avl_node *parent;

  size_t height;

  int key;
  int val;
};

avl_tree *avl_create(cmp_fn cmp_key, free_fn free_key, free_fn free_val);
void avl_destroy(avl_tree *avl);
bool avl_insert(avl_tree *avl, void *key, void *val);
avl_node *avl_find(avl_tree *avl, const void *key);
void avl_remove(avl_tree *avl, const void *key);
size_t avl_size(avl_tree *avl);
bool avl_valid_avl(avl_tree *avl);

avl_node *avl_first(avl_tree *avl);
avl_node *avl_last(avl_tree *avl);
avl_node *avl_next(avl_node *node);
avl_node *avl_prev(avl_node *node);

#define AVL_FOR_EACH(avl, KeyT, ValT, keyname, valname)                        \
  for (bool _should_loop = true, _should_break = false; _should_loop;          \
       _should_loop = false)                                                   \
    for (avl_node *_node = avl_first(avl); _node && !_should_break;            \
         _node = _should_break ? _node : avl_next(_node), _should_loop = true) \
      for (KeyT keyname = (KeyT)_node->key; _should_loop;                      \
           _should_break = _should_loop, _should_loop = false)                 \
        for (ValT valname = (ValT)_node->val; _should_loop;                    \
             _should_loop = false)

#define AVL_FOR_EACH_IT(avl, node_name)                                        \
  for (avl_node *node_name = avl_first(avl); node_name;                        \
       node_name = avl_next(node_name))

#ifdef __cplusplus
}
#endif

#endif /* BINARY_SEARCH_TREE_H */
