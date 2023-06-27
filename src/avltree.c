#include "datastructs/avl_tree.h"
#include "datastructs/functions.h"
#include "datastructs/macros.h"
#include "datastructs/memory.h"

#include <assert.h>
#include <stdlib.h>

static const int threshold = 1;

struct avl_tree {
  struct avl_node *root;
  cmp_fn cmp;
  free_fn free_key;
  free_fn free_val;
  size_t size;
};

static struct avl_node *avl_node_create(void *key, void *val,
                                        struct avl_node *parent) {
  struct avl_node *node = yu_allocate(sizeof(*node));
  if (!node) {
    return NULL;
  }
  node->left = NULL;
  node->right = NULL;
  node->parent = parent;

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
  return bheight(node->left) - bheight(node->right);
}

static inline int avl_height(struct avl_node *node) {
  return 1 + YU_MAX(bheight(node->left), bheight(node->right));
}

static struct avl_node *left_rotate(struct avl_node *node) {
  struct avl_node *rnode = node->right;
  node->right = rnode->left;
  rnode->left = node;

  rnode->parent = node->parent;
  node->parent = rnode;
  if (node->right)
    node->right->parent = node;

  node->height = avl_height(node);
  rnode->height = avl_height(rnode);
  return rnode;
}

static struct avl_node *right_rotate(struct avl_node *node) {
  struct avl_node *lnode = node->left;
  node->left = lnode->right;
  lnode->right = node;

  lnode->parent = node->parent;
  node->parent = lnode;
  if (node->left)
    node->left->parent = node;

  node->height = avl_height(node);
  lnode->height = avl_height(lnode);
  return lnode;
}

static struct avl_node **left_most(struct avl_node **node) {
  while ((*node)->left) {
    node = &(*node)->left;
  }
  return node;
}

static struct avl_node *change_child(struct avl_node *parent,
                                     struct avl_node *old, struct avl_node *new,
                                     struct avl_node **root) {
  if (parent) {
    if (parent->left == old) {
      parent->left = new;
    } else {
      parent->right = new;
    }

  } else {
    *root = new;
  }
  return new;
}

static struct avl_node *rebalance(struct avl_node **root,
                                  struct avl_node *node) {
  int balance = deviation(node);
  struct avl_node *parent = node->parent;
  if (balance > threshold) {
    /* left-right heavy? */
    if (deviation(node->left) < 0) {
      node->left = left_rotate(node->left);
    }
    return change_child(parent, node, right_rotate(node), root);

  } else if (balance < -threshold) {
    /* right-left heavy? */
    if (deviation(node->right) > 0) {
      node->right = right_rotate(node->right);
    }
    return change_child(parent, node, left_rotate(node), root);
  }
  node->height = avl_height(node);
  return node;
}

static void restore_avl_property(struct avl_node **root,
                                 struct avl_node *node) {
  if (!node) {
    return;
  }

  size_t height;
  do {
    height = node->height;
    node = rebalance(root, node)->parent;
  } while (node && height != node->height);
}

static inline bool balanced(struct avl_node *node) {
  return YU_ABS(deviation(node)) <= threshold;
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

struct avl_node *remove_node(avl_tree *avl, struct avl_node **target,
                             struct avl_node **destroy, struct avl_node *node) {
  if (!node->left || !node->right) {
    *target = node->left ? node->left : node->right;

    *destroy = node;
    node = node->parent;
  } else {
    target = left_most(&node->right);
    node->key = (*target)->key;
    node->val = (*target)->val;

    node = (*target)->parent;
    *destroy = *target;
    *target = (*target)->right;
  }

  if (*target)
    (*target)->parent = node;

  avl_node_destroy(avl, *destroy);
  return node;
}

void avl_remove(avl_tree *avl, const void *key) {
  assert(avl != NULL);

  struct avl_node **target = &avl->root;
  struct avl_node *destroy = NULL;
  struct avl_node *node;

  while (*target) {
    node = *target;
    int c = avl->cmp(key, node->key);
    if (c < 0) {
      target = &node->left;
    } else if (c > 0) {
      target = &node->right;
    } else { /* Key found */
      node = remove_node(avl, target, &destroy, node);
      break;
    }
  }

  if (destroy) {
    avl->size--;
    restore_avl_property(&avl->root, node);
  }
}

bool avl_insert(avl_tree *avl, void *key, void *val) {
  assert(avl != NULL);

  struct avl_node **link = &avl->root;
  struct avl_node *parent = NULL;

  while (*link) {
    parent = *link;
    int c = avl->cmp(key, parent->key);
    if (c < 0) {
      link = &parent->left;
    } else if (c > 0) {
      link = &parent->right;
    } else {
      return false;
    }
  }
  avl->size++;
  *link = avl_node_create(key, val, parent);

  restore_avl_property(&avl->root, parent);
  return true;
}

struct avl_node *avl_find(avl_tree *avl, const void *key) {
  assert(avl != NULL);

  struct avl_node *node = avl->root;
  while (node) {
    int c = avl->cmp(key, node->key);
    if (c < 0) {
      node = node->left;
    } else if (c > 0) {
      node = node->right;
    } else {
      return node;
    }
  }
  return NULL;
}

bool avl_valid_avl(avl_tree *avl) {
  assert(avl != NULL);
  return valid_avl(avl->root);
}

size_t avl_size(avl_tree *avl) {
  assert(avl != NULL);
  return avl->size;
}

struct avl_node *avl_first(avl_tree *avl) {
  assert(avl != NULL);
  return *left_most(&avl->root);
}

struct avl_node *avl_last(avl_tree *avl) {
  assert(avl != NULL);

  struct avl_node *node = avl->root;
  while (node->right) {
    node = node->right;
  }
  return node;
}

struct avl_node *avl_next(struct avl_node *node) {
  assert(node != NULL);

  struct avl_node *parent;
  if (node->right) {
    node = node->right;
    while (node->left) {
      node = node->left;
    }
    return node;
  }

  while ((parent = node->parent) && node == parent->right) {
    node = parent;
  }
  return parent;
}

struct avl_node *avl_prev(struct avl_node *node) {
  assert(node != NULL);

  struct avl_node *parent;
  if (node->left) {
    node = node->left;
    while (node->right) {
      node = node->right;
    }
    return node;
  }

  while ((parent = node->parent) && node == parent->left) {
    node = parent;
  }
  return parent;
}
