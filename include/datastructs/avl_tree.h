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

struct avl_tree {
  struct avl_root root;

  compare_avl_nodes_fun cmp;

  size_t num_items;
};

#define avl_entry(ptr, type, member) YU_CONTAINER_OF(ptr, type, member)
#define avl_entry_safe(ptr, type, member)                                      \
  ({                                                                           \
    __typeof__(ptr) _ptr = (ptr);                                              \
    _ptr ? avl_entry(_ptr, type, member) : NULL;                               \
  })

void avl_init(avl_tree *avl, compare_avl_nodes_fun cmp);

struct avl_node *avl_insert(avl_tree *avl, struct avl_node *node);
struct avl_node *avl_remove(avl_tree *avl, const struct avl_node *query);
struct avl_node *avl_find(avl_tree *avl, const struct avl_node *query);

struct avl_node *avl_remove_node(struct avl_root *root, struct avl_node **link);
void avl_restore_properties(struct avl_root *root, struct avl_node *node);
void avl_link_node(struct avl_node *node, struct avl_node *parent,
                   struct avl_node **link);

struct avl_node *avl_first_postorder(const struct avl_root *root);
struct avl_node *avl_next_postorder(const struct avl_node *node);

struct avl_node *avl_first(const struct avl_root *root);
struct avl_node *avl_last(const struct avl_root *root);
struct avl_node *avl_next(const struct avl_node *node);
struct avl_node *avl_prev(const struct avl_node *node);

#define AVL_FOR_EACH(root, cur, field)                                         \
  for (cur = avl_entry_safe(avl_first(root), __typeof__(*cur), field); cur;    \
       cur = avl_entry_safe(avl_next(&cur->field), __typeof__(*cur), field))

#define AVL_POSTORDER_FOR_EACH(root, cur, n, field)                            \
  for (cur =                                                                   \
           avl_entry_safe(avl_first_postorder(root), __typeof__(*cur), field); \
       cur && ({                                                               \
         n = avl_entry_safe(avl_next_postorder(&cur->field), __typeof__(*cur), \
                            field);                                            \
         1;                                                                    \
       });                                                                     \
       cur = n)

#define AVL_FOR_EACH_NODE(root, node)                                          \
  for (struct avl_node *node = avl_first(root); node; node = avl_next(node))

#ifdef __cplusplus
}
#endif

#endif /* YU_AVL_TREE_H */
