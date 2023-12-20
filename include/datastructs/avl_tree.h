#ifndef YU_AVL_TREE_H
#define YU_AVL_TREE_H

#include "macros.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct avl_root {
  struct avl_node *avl_node;
};

struct avl_node {
  struct avl_node *left;
  struct avl_node *right;
  struct avl_node *parent;

  size_t height;
};

typedef int (*avl_compare_fun)(const struct avl_node *,
                               const struct avl_node *);

#define avl_entry(ptr, type, member) YU_CONTAINER_OF(ptr, type, member)
#define avl_entry_safe(ptr, type, member)                                      \
  YU_CONTAINER_OF_SAFE(ptr, type, member)

void avl_restore_properties(struct avl_node *rebalance, struct avl_root *root);

struct avl_node *avl_insert(struct avl_node *node, struct avl_root *root,
                            avl_compare_fun cmp);
struct avl_node *avl_find(const struct avl_node *query, struct avl_root *root,
                          avl_compare_fun cmp);

void avl_link_node(struct avl_node *node, struct avl_node *parent,
                   struct avl_node **link);
void avl_erase(struct avl_node *node, struct avl_root *root);

struct avl_node *avl_first_postorder(const struct avl_root *root);
struct avl_node *avl_next_postorder(const struct avl_node *node);

struct avl_node *avl_first(const struct avl_root *root);
struct avl_node *avl_last(const struct avl_root *root);
struct avl_node *avl_next(const struct avl_node *node);
struct avl_node *avl_prev(const struct avl_node *node);

#define avl_for_each(root, cur, field)                                         \
  for (cur = avl_entry_safe(avl_first(root), yu_typeof(*cur), field); cur;     \
       cur = avl_entry_safe(avl_next(&cur->field), yu_typeof(*cur), field))

#define avl_postorder_for_each(root, cur, n, field)                            \
  for (cur =                                                                   \
           avl_entry_safe(avl_first_postorder(root), yu_typeof(*cur), field);  \
       cur && ((n = avl_entry_safe(avl_next_postorder(&cur->field),            \
                                   yu_typeof(*cur), field)) ||                 \
               1);                                                             \
       cur = n)

#ifdef __cplusplus
}
#endif

#endif /* YU_AVL_TREE_H */
