#include "datastructs/avl_tree.h"
#include "datastructs/functions.h"
#include "datastructs/macros.h"
#include "datastructs/memory.h"

#include <assert.h>
#include <stdlib.h>

static const int threshold = 1;

struct avl_node {
  struct avl_node *left;
  struct avl_node *right;
  void *key;
  void *val;
  size_t height;
};

struct avl_tree {
  struct avl_node *root;
  cmp_fn cmp;
  free_fn free_key;
  free_fn free_val;
  size_t size;
};

struct avl_iterator {
  struct avl_node **parents;
  struct avl_node **par_sp;
};

static struct avl_node *avl_node_create(void *key, void *val) {
  struct avl_node *node = yu_allocate(sizeof(*node));
  if (!node) {
    return NULL;
  }
  node->left = NULL;
  node->right = NULL;
  node->key = key;
  node->val = val;
  node->height = 1;
  return node;
}

static inline void avl_node_destroy(avl_tree *avl, struct avl_node *node) {
  avl->free_key(node->key);
  avl->free_val(node->val);
  free(node);
}

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

  node->height = height(node);
  rnode->height = height(rnode);
  return rnode;
}

static struct avl_node *right_rotate(struct avl_node *node) {
  struct avl_node *lnode = node->left;
  node->left = lnode->right;
  lnode->right = node;

  node->height = height(node);
  lnode->height = height(lnode);
  return lnode;
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
  node->height = height(node);
  return node;
}

static struct avl_node *insert_node(avl_tree *avl, struct avl_node *node,
                                    void *key, void *val) {
  if (!node) {
    avl->size++;
    return avl_node_create(key, val);
  }
  int c = avl->cmp(key, node->key);
  if (c < 0) {
    node->left = insert_node(avl, node->left, key, val);
  } else if (c > 0) {
    node->right = insert_node(avl, node->right, key, val);
  } else {
    return node; /* Already exist */
  }
  /* Balance case */
  return balance(node);
}

static struct avl_node *remove_node(avl_tree *avl, struct avl_node *node,
                                    const void *key) {
  if (!node) {
    return NULL;
  }
  int c = avl->cmp(key, node->key);
  if (c < 0) {
    node->left = remove_node(avl, node->left, key);
  } else if (c > 0) {
    node->right = remove_node(avl, node->right, key);
  } else { /* Key found */
    struct avl_node *ret;
    if (!node->left || !node->right) {
      ret = node->left ? node->left : node->right;
      avl->size--;
      avl_node_destroy(avl, node);
      return ret;
    }
    ret = left_most(node->right);
    node->key = ret->key;
    node->right = remove_node(avl, node->right, ret->key);
  }
  /* Balance case */
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

static void avl_free_rec(avl_tree *avl, struct avl_node *node) {
  if (!node) {
    return;
  }
  avl_free_rec(avl, node->left);
  avl_free_rec(avl, node->right);
  avl_node_destroy(avl, node);
}

avl_tree *avl_create(cmp_fn cmp_key, free_fn free_key, free_fn free_val) {
  assert(cmp_key != NULL);

  avl_tree *avl = yu_allocate(sizeof(*avl));
  if (!avl) {
    return NULL;
  }

  avl->cmp = cmp_key;
  avl->free_key = free_key ? free_key : free_placeholder;
  avl->free_val = free_val ? free_val : free_placeholder;
  avl->root = NULL;
  avl->size = 0;
  return avl;
}

void avl_destroy(avl_tree *avl) {
  if (avl) {
    avl_free_rec(avl, avl->root);
    free(avl);
  }
}

void avl_remove(avl_tree *avl, const void *key) {
  assert(avl != NULL);
  avl->root = remove_node(avl, avl->root, key);
}

void avl_insert(avl_tree *avl, void *key, void *val) {
  assert(avl != NULL);
  avl->root = insert_node(avl, avl->root, key, val);
}

bool avl_find(avl_tree *avl, const void *key) {
  assert(avl != NULL);

  struct avl_node *walk = avl->root;
  while (walk) {
    int c = avl->cmp(key, walk->key);
    if (c < 0) {
      walk = walk->left;
    } else if (c > 0) {
      walk = walk->right;
    } else {
      return true;
    }
  }
  return false;
}

bool avl_valid_avl(avl_tree *avl) {
  assert(avl != NULL);
  return valid_avl(avl->root);
}

size_t avl_size(avl_tree *avl) {
  assert(avl != NULL);
  return avl->size;
}

#define STACK_PUSH(node) (*(it->par_sp++) = (node))
#define STACK_POP() (*(--it->par_sp))
#define STACK_TOP() ((it->par_sp[-1]))

avl_iterator *avl_first(avl_tree *avl) {
  assert(avl != NULL);

  avl_iterator *it = yu_allocate(sizeof(*it));
  if (!it) {
    return NULL;
  }
  it->parents = yu_allocate(sizeof(*it->parents) * (avl->root->height + 2));
  if (!it->parents) {
    free(it);
    return NULL;
  }
  it->par_sp = it->parents;

  STACK_PUSH(NULL);
  struct avl_node *node = avl->root;
  while (node) {
    STACK_PUSH(node);
    node = node->left;
  }
  return it;
}

void avl_it_destroy(avl_iterator *it) {
  if (it) {
    free(it->parents);
    free(it);
  }
}

void avl_next(avl_iterator *it) {
  assert(it != NULL);
  struct avl_node *node = STACK_TOP();
  struct avl_node *parent;
  if (node->right) {
    node = node->right;
    STACK_PUSH(node);
    while (node->left) {
      node = node->left;
      STACK_PUSH(node);
    }
    return;
  }

  STACK_POP();
  while ((parent = STACK_TOP()) && node == parent->right) {
    node = parent;
    STACK_POP();
  }
}

void avl_prev(avl_iterator *it) {
  assert(it != NULL);
  struct avl_node *node = STACK_TOP();
  struct avl_node *parent;
  if (node->left) {
    node = node->left;
    STACK_PUSH(node);
    while (node->right) {
      node = node->right;
      STACK_PUSH(node);
    }
    return;
  }

  STACK_POP();
  while ((parent = STACK_TOP()) && node == parent->left) {
    node = parent;
    STACK_POP();
  }
}

bool avl_has_next(avl_iterator *it) {
  assert(it != NULL);
  return STACK_TOP();
}

struct key_value avl_get(avl_iterator *it) {
  assert(it != NULL);
  return (struct key_value){
      .key = STACK_TOP()->key,
      .val = STACK_TOP()->val,
  };
}
