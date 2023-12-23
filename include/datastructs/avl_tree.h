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

/**
 * @brief Restore AVL properties
 *
 * Should be called right after insertion or removal of item
 * See examples
 *
 * @param rebalance Node to start rebalance from
 * @param root Root of AVL tree
 */
void avl_restore_properties(struct avl_node *rebalance, struct avl_root *root);

/**
 * @brief Insert node into AVL tree
 *
 * @param node Node to insert
 * @param root Root of AVL tree
 * @param cmp Function to compare two noods
 */
struct avl_node *avl_insert(struct avl_node *node, struct avl_root *root,
                            avl_compare_fun cmp);
/**
 * @brief Find node using query
 *
 * @param query Query
 * @param root Root of AVL tree
 * @param cmp Function to compare two nodes
 */
struct avl_node *avl_find(const struct avl_node *query, struct avl_root *root,
                          avl_compare_fun cmp);
/**
 * @brief Link node
 *
 * Must be called after every insertion
 * See examples
 *
 * @param node Node
 * @param parent Parent node
 * @param link Link
 */
void avl_link_node(struct avl_node *node, struct avl_node *parent,
                   struct avl_node **link);
/**
 * @brief Erase node
 *
 * @param node Node to erase
 * @param root Root of AVL tree
 */
void avl_erase(struct avl_node *node, struct avl_root *root);

/**
 * @brief First postorder node
 *
 * @param root Root of AVL tree
 */
struct avl_node *avl_first_postorder(const struct avl_root *root);
/**
 * @brief Next postorder node
 *
 * @param node Node
 */
struct avl_node *avl_next_postorder(const struct avl_node *node);

/**
 * @brief First inorder node
 *
 * @param root Root of AVL tree
 */
struct avl_node *avl_first(const struct avl_root *root);
/**
 * @brief Last inorder node
 *
 * @param root Root of AVL tree
 */
struct avl_node *avl_last(const struct avl_root *root);
/**
 * @brief Next inorder node
 *
 * @param node Node
 */
struct avl_node *avl_next(const struct avl_node *node);
/**
 * @brief Previous inorder node
 *
 * @param node Node
 */
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
