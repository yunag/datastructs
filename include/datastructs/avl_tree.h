#ifndef YU_AVL_TREE_H
#define YU_AVL_TREE_H

#include "datastructs/macros.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct avl_tree avl_tree;

struct avl_root {
  struct avl_node *avl_node;
};

struct avl_node {
  struct avl_node *left;
  struct avl_node *right;
  struct avl_node *parent;

  size_t height;
};

typedef int (*compare_avl_nodes_fun)(const struct avl_node *,
                                     const struct avl_node *);
typedef void (*destroy_avl_node_fun)(struct avl_node *);

struct avl_tree {
  struct avl_root root;

  compare_avl_nodes_fun cmp;
  destroy_avl_node_fun destroy;

  size_t size;
};

#define avl_entry(ptr, type, member) YU_CONTAINER_OF(ptr, type, member)

void avl_init(avl_tree *avl, compare_avl_nodes_fun cmp,
              destroy_avl_node_fun destroy);
void avl_uninit(avl_tree *avl);

bool avl_insert(avl_tree *avl, struct avl_node *node);
void avl_remove(avl_tree *avl, const struct avl_node *query);
struct avl_node *avl_find(avl_tree *avl, const struct avl_node *query);

struct avl_node *avl_remove_node(struct avl_root *root, struct avl_node **link);
void avl_restore_properties(struct avl_root *root, struct avl_node *node);
void avl_link_node(struct avl_node *node, struct avl_node *parent,
                   struct avl_node **link);

struct avl_node *avl_first(const struct avl_root *root);
struct avl_node *avl_last(const struct avl_root *root);
struct avl_node *avl_next(const struct avl_node *node);
struct avl_node *avl_prev(const struct avl_node *node);

#define AVL_FOR_EACH(root, node)                                               \
  for (struct avl_node *node = avl_first(root); node; node = avl_next(node))

#ifdef __cplusplus
}
#endif

#endif /* YU_AVL_TREE_H */
