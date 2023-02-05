#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include <datastructs/types.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bst bst;

bst *bst_create(void);
void bst_free(bst *binary_search_tree);
void bst_insert(bst *binary_search_tree, int key);
bool bst_find(bst *binary_search_tree, int key);
void bst_remove(bst *binary_search_tree, int key);
void bst_inorder_print(bst *binary_search_tree);
size_t bst_size(bst *binary_search_tree);
bool bst_valid_avl(bst *binary_search_tree);
bool bst_valid_bst(bst *binary_search_tree);

#ifdef __cplusplus
}
#endif

#endif /* BINARY_SEARCH_TREE_H */
