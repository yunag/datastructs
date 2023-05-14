#include <datastructs/bst.h>
#include <datastructs/macros.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const int threshold = 1;

struct bnode {
  struct bnode *left;
  struct bnode *right;
  int key;
  int height;
};

struct bst {
  struct bnode *root;
  size_t size;
};

static struct bnode *bnode_create(struct bnode *left, struct bnode *right,
                                  int key) {
  struct bnode *bnode = malloc(sizeof(*bnode));
  if (!bnode) {
    YU_LOG_ERROR("Failed to allocate memory for node");
    return NULL;
  }
  bnode->left = left;
  bnode->right = right;
  bnode->key = key;
  bnode->height = 1;
  return bnode;
}

static inline void bnode_free(struct bnode *node) { free(node); }

static inline int bheight(struct bnode *node) {
  return !node ? 0 : node->height;
}

static inline int deviation(struct bnode *node) {
  return !node ? 0 : bheight(node->left) - bheight(node->right);
}

static inline int height(struct bnode *node) {
  return 1 + YU_MAX(bheight(node->left), bheight(node->right));
}

static struct bnode *left_rotate(struct bnode *node) {
  struct bnode *rnode = node->right;
  node->right = rnode->left;
  rnode->left = node;
  /* NOTE: The order in which the heights are counted matters:
   * the node will become a child of the right node */
  node->height = height(node);
  rnode->height = height(rnode);
  return rnode;
}

static struct bnode *right_rotate(struct bnode *node) {
  struct bnode *lnode = node->left;
  node->left = lnode->right;
  lnode->right = node;
  /* NOTE: The order in which the heights are counted matters:
   * the node will become a child of the left node */
  node->height = height(node);
  lnode->height = height(lnode);
  return lnode;
}

static void inorder_print(struct bnode *node) {
  if (node) {
    inorder_print(node->left);
    fprintf(stdout, "%d ", node->key);
    inorder_print(node->right);
  }
}

static struct bnode *min_node(struct bnode *node) {
  while (node->left) {
    node = node->left;
  }
  return node;
}

static struct bnode *balance(struct bnode *node) {
  int balance = deviation(node);
  if (balance > threshold) {
    /* The left node has a left subtree that is havier than the right subtree */
    if (deviation(node->left) >= 0) {
      return right_rotate(node);
    } else { /* < 0 */
      node->left = left_rotate(node->left);
      return right_rotate(node);
    }
  } else if (balance < -threshold) {
    /* The right node has a right subtree that is heavier than the left subtree
     */
    if (deviation(node->right) <= 0) {
      return left_rotate(node);
    } else { /* > 0 */
      node->right = right_rotate(node->right);
      return left_rotate(node);
    }
  }
  return node;
}

static struct bnode *insert_bnode(bst *_bst, struct bnode *node, int key) {
  if (!node) {
    _bst->size++;
    return bnode_create(NULL, NULL, key);
  }
  if (key < node->key) {
    node->left = insert_bnode(_bst, node->left, key);
  } else if (key > node->key) {
    node->right = insert_bnode(_bst, node->right, key);
  } else {
    return node; /* Already exist */
  }
  /* Balance case */
  node->height = height(node);
  return balance(node);
}

static struct bnode *remove_node(bst *_bst, struct bnode *node, int key) {
  if (!node) {
    return NULL;
  }

  if (key < node->key) {
    node->left = remove_node(_bst, node->left, key);
  } else if (key > node->key) {
    node->right = remove_node(_bst, node->right, key);
  } else { /* Key found */
    struct bnode *ret;
    if (!node->left || !node->right) {
      ret = node->left ? node->left : node->right;
      _bst->size--;
      bnode_free(node);
      return ret;
    }
    ret = min_node(node->right);
    node->key = ret->key;
    node->right = remove_node(_bst, node->right, ret->key);
  }
  /* Balance case */
  node->height = height(node);
  return balance(node);
}

static inline bool balanced(struct bnode *node) {
  int balance = deviation(node);
  return YU_ABS(balance) <= threshold;
}

static bool valid_avl(struct bnode *node) {
  if (!node) {
    return true;
  }
  if (!balanced(node)) {
    return false;
  }
  return valid_avl(node->left) && valid_avl(node->right);
}

static void bst_free_tree(struct bnode *node) {
  if (!node) {
    return;
  }
  bst_free_tree(node->left);
  bst_free_tree(node->right);
  bnode_free(node);
}

bst *bst_create(void) {
  bst *_bst = malloc(sizeof(*_bst));
  if (!_bst) {
    YU_LOG_ERROR("Failed to allocate memory for binary search tree");
    return NULL;
  }
  _bst->root = NULL;
  _bst->size = 0;
  return _bst;
}

void bst_destroy(bst *_bst) {
  if (_bst) {
    bst_free_tree(_bst->root);
    free(_bst);
  }
}

void bst_remove(bst *_bst, int key) {
  assert(_bst != NULL);
  _bst->root = remove_node(_bst, _bst->root, key);
}

void bst_insert(bst *_bst, int key) {
  assert(_bst != NULL);
  _bst->root = insert_bnode(_bst, _bst->root, key);
}

bool bst_find(bst *_bst, int key) {
  assert(_bst != NULL);
  struct bnode *walk = _bst->root;
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

void bst_inorder_print(bst *_bst) {
  assert(_bst != NULL);
  inorder_print(_bst->root);
  if (_bst->root) {
    printf("\b\n");
  } else {
    printf("\n");
  }
}

bool bst_valid_avl(bst *_bst) {
  assert(_bst != NULL);
  return valid_avl(_bst->root);
}

static bool valid_bst(struct bnode *node, int64_t left, int64_t right) {
  if (!node) {
    return true;
  }
  if (node->key >= right || node->key <= left) {
    return false;
  }
  return valid_bst(node->left, left, node->key) &&
         valid_bst(node->right, node->key, right);
}

bool bst_valid_bst(bst *_bst) {
  assert(_bst != NULL);
  return valid_bst(_bst->root, INT_MIN, INT_MAX);
}

size_t bst_size(bst *_bst) {
  assert(_bst != NULL);
  return _bst->size;
}
