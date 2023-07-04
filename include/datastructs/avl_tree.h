#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include "datastructs/macros.h"
#include "datastructs/types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct avl_tree avl_tree;

#define avl_entry(ptr, type, member) YU_CONTAINER_OF(ptr, type, member)

struct avl_root {
  struct avl_node *avl_node;
};

struct avl_node {
  struct avl_node *left;
  struct avl_node *right;
  struct avl_node *parent;

  size_t height;
};

typedef int (*cmp_avl_nodes_fn)(const struct avl_node *,
                                const struct avl_node *);
typedef void (*destroy_avl_node_fn)(struct avl_node *);

avl_tree *avl_create(cmp_avl_nodes_fn cmp, destroy_avl_node_fn destroy);
void avl_destroy(avl_tree *avl);
bool avl_insert(avl_tree *avl, struct avl_node *node);
struct avl_node *avl_find(avl_tree *avl, const struct avl_node *query);
void avl_remove(avl_tree *avl, const struct avl_node *query);
size_t avl_size(avl_tree *avl);
struct avl_node *avl_root(avl_tree *avl);
static void avl_restore_properties(struct avl_root *root,
                                   struct avl_node *node);
void avl_link_node(struct avl_node *node, struct avl_node *parent,
                   struct avl_node **link);

struct avl_node *avl_remove_node(struct avl_root *root, struct avl_node **link);
struct avl_node *avl_first(avl_tree *avl);
struct avl_node *avl_last(avl_tree *avl);
struct avl_node *avl_next(struct avl_node *node);
struct avl_node *avl_prev(struct avl_node *node);

#define AVL_FOR_EACH(avl, node_name)                                           \
  for (avl_node *node_name = avl_first(avl); node_name;                        \
       node_name = avl_next(node_name))

#ifdef __cplusplus
}
#endif

#endif /* BINARY_SEARCH_TREE_H */
