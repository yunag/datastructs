#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <datastructs/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct avl_tree avl_tree;

avl_tree *avl_create(void);
void avl_destroy(avl_tree *avl);
void avl_insert(avl_tree *avl, int key);
bool avl_find(avl_tree *avl, int key);
void avl_remove(avl_tree *avl, int key);
void avl_inorder_print(avl_tree *avl);
size_t avl_size(avl_tree *avl);
bool avl_valid_avl(avl_tree *avl);
bool avl_valid_bst(avl_tree *avl);

#ifdef __cplusplus
}
#endif

#endif /* BINARY_SEARCH_TREE_H */
