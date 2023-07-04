#include "datastructs/avl_tree.h"
#include "datastructs/macros.h"
#include "datastructs/memory.h"

#include <assert.h>
#include <stdlib.h>

static const int threshold = 1;

struct avl_tree {
  struct avl_root root;

  cmp_avl_nodes_fn cmp;
  destroy_avl_node_fn destroy;

  size_t size;
};

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

void avl_link_node(struct avl_node *node, struct avl_node *parent,
                   struct avl_node **link) {
  node->left = node->right = NULL;
  node->parent = parent;
  node->height = 1;

  *link = node;
}

void avl_change_child(struct avl_node *parent, struct avl_node *old,
                      struct avl_node *new, struct avl_root *root) {
  if (parent) {
    if (parent->left == old) {
      parent->left = new;
    } else {
      parent->right = new;
    }

  } else {
    root->avl_node = new;
  }
}

static struct avl_node *rebalance(struct avl_root *root,
                                  struct avl_node *node) {
  struct avl_node *parent = node->parent;
  struct avl_node *new = node;

  size_t height = node->height;
  int balance = deviation(node);

  if (balance > threshold) {
    /* left-right heavy? */
    if (deviation(node->left) < 0) {
      node->left = left_rotate(node->left);
    }
    avl_change_child(parent, node, new = right_rotate(node), root);

  } else if (balance < -threshold) {
    /* right-left heavy? */
    if (deviation(node->right) > 0) {
      node->right = right_rotate(node->right);
    }
    avl_change_child(parent, node, new = left_rotate(node), root);
  } else {
    node->height = avl_height(node);
  }
  return height != new->height ? parent : NULL;
}

void avl_restore_properties(struct avl_root *root, struct avl_node *node) {
  while (node) {
    node = rebalance(root, node);
  }
}

avl_tree *avl_create(cmp_avl_nodes_fn cmp, destroy_avl_node_fn destroy) {
  assert(cmp != NULL);

  avl_tree *avl = yu_allocate(sizeof(*avl));
  if (!avl) {
    return NULL;
  }

  avl->cmp = cmp;
  avl->destroy = destroy;
  avl->root.avl_node = NULL;
  avl->size = 0;
  return avl;
}

static void avl_free_rec(avl_tree *avl, struct avl_node *node) {
  if (!node) {
    return;
  }
  avl_free_rec(avl, node->left);
  avl_free_rec(avl, node->right);
  avl->destroy(node);
}

void avl_destroy(avl_tree *avl) {
  if (!avl) {
    return;
  }
  if (avl->destroy) {
    avl_free_rec(avl, avl->root.avl_node);
  }
  yu_free(avl);
}

void avl_replace_node(struct avl_node *victim, struct avl_node *new,
                      struct avl_root *root) {
  struct avl_node *parent = victim->parent;

  *new = *victim;
  if (victim->left) {
    victim->left->parent = new;
  }
  if (victim->right) {
    victim->right->parent = new;
  }
  avl_change_child(parent, victim, new, root);
}

struct avl_node *avl_remove_node(struct avl_root *root,
                                 struct avl_node **link) {
  struct avl_node *node = *link;
  struct avl_node *victim = node;
  struct avl_node *new = node->parent;

  if (!node->left) {
    *link = node->right;
  } else if (!node->right) {
    *link = node->left;
  } else {
    link = &node->right;
    while ((*link)->left) {
      link = &(*link)->left;
    }

    node = *link;
    *link = (*link)->right;

    new = node->parent == victim ? node : node->parent;
    avl_replace_node(victim, node, root);
  }
  if (*link) {
    (*link)->parent = new;
  }
  return new;
}

static struct avl_node *avl_remove_node_internal(avl_tree *avl,
                                                 struct avl_node **link) {
  struct avl_node *node = *link;
  struct avl_node *new = avl_remove_node(&avl->root, link);
  if (avl->destroy) {
    avl->destroy(node);
  }
  avl->size--;
  return new;
}

void avl_remove(avl_tree *avl, const struct avl_node *query) {
  assert(avl != NULL);

  struct avl_node **link = &avl->root.avl_node;
  struct avl_node *node = NULL;

  while (*link) {
    int c = avl->cmp(query, *link);
    if (c < 0) {
      link = &(*link)->left;
    } else if (c > 0) {
      link = &(*link)->right;
    } else {
      node = avl_remove_node_internal(avl, link);
      break;
    }
  }

  avl_restore_properties(&avl->root, node);
}

bool avl_insert(avl_tree *avl, struct avl_node *node) {
  assert(avl != NULL);

  struct avl_node **link = &avl->root.avl_node;
  struct avl_node *parent = NULL;

  while (*link) {
    parent = *link;
    int c = avl->cmp(node, parent);
    if (c < 0) {
      link = &parent->left;
    } else if (c > 0) {
      link = &parent->right;
    } else {
      return false;
    }
  }
  avl->size++;
  avl_link_node(node, parent, link);
  avl_restore_properties(&avl->root, parent);
  return true;
}

struct avl_node *avl_find(avl_tree *avl, const struct avl_node *query) {
  assert(avl != NULL);

  struct avl_node *node = avl->root.avl_node;

  while (node) {
    int c = avl->cmp(query, node);
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

size_t avl_size(avl_tree *avl) {
  assert(avl != NULL);
  return avl->size;
}

struct avl_node *avl_root(avl_tree *avl) {
  assert(avl != NULL);
  return avl->root.avl_node;
}

struct avl_node *avl_first(avl_tree *avl) {
  assert(avl != NULL);

  struct avl_node *node = avl->root.avl_node;
  if (!node) {
    return NULL;
  }
  while (node->left) {
    node = node->left;
  }
  return node;
}

struct avl_node *avl_last(avl_tree *avl) {
  assert(avl != NULL);

  struct avl_node *node = avl->root.avl_node;
  if (!node) {
    return NULL;
  }
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
