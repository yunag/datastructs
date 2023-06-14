#include <datastructs/avl_tree.h>
#include <datastructs/macros.h>

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static const int threshold = 1;

struct avl_node {
  struct avl_node *left;
  struct avl_node *right;
  int key;
  int height;
};

struct avl_tree {
  struct avl_node *root;
  size_t size;
};

static struct avl_node *avl_node_create(struct avl_node *left,
                                        struct avl_node *right, int key) {
  struct avl_node *node = malloc(sizeof(*node));
  if (!node) {
    YU_LOG_ERROR("Failed to allocate memory for node");
    return NULL;
  }
  node->left = left;
  node->right = right;
  node->key = key;
  node->height = 1;
  return node;
}

static inline void avl_node_destroy(struct avl_node *node) { free(node); }

static inline int bheight(struct avl_node *node) {
  return !node ? 0 : node->height;
}

static inline int deviation(struct avl_node *node) {
  return !node ? 0 : bheight(node->left) - bheight(node->right);
}

static inline int height(struct avl_node *node) {
  return 1 + YU_MAX(bheight(node->left), bheight(node->right));
}

static struct avl_node *left_rotate(struct avl_node *node) {
  struct avl_node *rnode = node->right;
  node->right = rnode->left;
  rnode->left = node;
  /* NOTE: The order in which the heights are counted matters:
   * the node will become a child of the right node */
  node->height = height(node);
  rnode->height = height(rnode);
  return rnode;
}

static struct avl_node *right_rotate(struct avl_node *node) {
  struct avl_node *lnode = node->left;
  node->left = lnode->right;
  lnode->right = node;
  /* NOTE: The order in which the heights are counted matters:
   * the node will become a child of the left node */
  node->height = height(node);
  lnode->height = height(lnode);
  return lnode;
}

static void inorder_print(struct avl_node *node) {
  if (node) {
    inorder_print(node->left);
    fprintf(stdout, "%d ", node->key);
    inorder_print(node->right);
  }
}

static struct avl_node *left_most(struct avl_node *node) {
  while (node->left) {
    node = node->left;
  }
  return node;
}

static struct avl_node *balance(struct avl_node *node) {
  int balance = deviation(node);
  if (balance > threshold) {
    /* The left node has a right subtree that is heavier than the left subtree
     */
    if (deviation(node->left) < 0) {
      node->left = left_rotate(node->left);
    }
    return right_rotate(node);

  } else if (balance < -threshold) {
    /* The right node has a left subtree that is heavier than the right subtree
     */
    if (deviation(node->right) > 0) {
      node->right = right_rotate(node->right);
    }
    return left_rotate(node);
  }
  return node;
}

static struct avl_node *insert_node(avl_tree *avl, struct avl_node *node,
                                    int key) {
  if (!node) {
    avl->size++;
    return avl_node_create(NULL, NULL, key);
  }
  if (key < node->key) {
    node->left = insert_node(avl, node->left, key);
  } else if (key > node->key) {
    node->right = insert_node(avl, node->right, key);
  } else {
    return node; /* Already exist */
  }
  /* Balance case */
  node->height = height(node);
  return balance(node);
}

static struct avl_node *remove_node(avl_tree *avl, struct avl_node *node,
                                    int key) {
  if (!node) {
    return NULL;
  }

  if (key < node->key) {
    node->left = remove_node(avl, node->left, key);
  } else if (key > node->key) {
    node->right = remove_node(avl, node->right, key);
  } else { /* Key found */
    struct avl_node *ret;
    if (!node->left || !node->right) {
      ret = node->left ? node->left : node->right;
      avl->size--;
      avl_node_destroy(node);
      return ret;
    }
    ret = left_most(node->right);
    node->key = ret->key;
    node->right = remove_node(avl, node->right, ret->key);
  }
  /* Balance case */
  node->height = height(node);
  return balance(node);
}

static inline bool balanced(struct avl_node *node) {
  int balance = deviation(node);
  return YU_ABS(balance) <= threshold;
}

static bool valid_avl(struct avl_node *node) {
  if (!node) {
    return true;
  }
  if (!balanced(node)) {
    return false;
  }
  return valid_avl(node->left) && valid_avl(node->right);
}

static void avl_free_rec(struct avl_node *node) {
  if (!node) {
    return;
  }
  avl_free_rec(node->left);
  avl_free_rec(node->right);
  avl_node_destroy(node);
}

avl_tree *avl_create(void) {
  avl_tree *avl = malloc(sizeof(*avl));
  if (!avl) {
    YU_LOG_ERROR("Failed to allocate memory for binary search tree");
    return NULL;
  }
  avl->root = NULL;
  avl->size = 0;
  return avl;
}

void avl_destroy(avl_tree *avl) {
  if (avl) {
    avl_free_rec(avl->root);
    free(avl);
  }
}

void avl_remove(avl_tree *avl, int key) {
  assert(avl != NULL);
  avl->root = remove_node(avl, avl->root, key);
}

void avl_insert(avl_tree *avl, int key) {
  assert(avl != NULL);
  avl->root = insert_node(avl, avl->root, key);
}

bool avl_find(avl_tree *avl, int key) {
  assert(avl != NULL);
  struct avl_node *walk = avl->root;
  while (walk) {
    if (key < walk->key) {
      walk = walk->left;
    } else if (key > walk->key) {
      walk = walk->right;
    } else {
      return true;
    }
  }
  return false;
}

void avl_inorder_print(avl_tree *avl) {
  assert(avl != NULL);
  inorder_print(avl->root);
  if (avl->root) {
    printf("\b\n");
  } else {
    printf("\n");
  }
}

bool avl_valid_avl(avl_tree *avl) {
  assert(avl != NULL);
  return valid_avl(avl->root);
}

static bool valid_bst(struct avl_node *node, int64_t left, int64_t right) {
  if (!node) {
    return true;
  }
  if (node->key >= right || node->key <= left) {
    return false;
  }
  return valid_bst(node->left, left, node->key) &&
         valid_bst(node->right, node->key, right);
}

bool avl_valid_bst(avl_tree *avl) {
  assert(avl != NULL);
  return valid_bst(avl->root, INT_MIN, INT_MAX);
}

size_t avl_size(avl_tree *avl) {
  assert(avl != NULL);
  return avl->size;
}
