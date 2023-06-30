#include "datastructs/avl_tree.h"
#include "datastructs/macros.h"
#include "datastructs/memory.h"

#include <assert.h>
#include <stdlib.h>

static const int threshold = 1;

struct avl_tree {
  struct avl_node *root;
  cmp_avl_nodes_fn cmp;
  destroy_avl_node_fn destroy;
  size_t size;
};

struct avl_node *avl_node_init(struct avl_node *node, struct avl_node *parent) {
  node->left = NULL;
  node->right = NULL;
  node->parent = parent;

  node->height = 1;
  return node;
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

static struct avl_node *avl_change_child(struct avl_node *parent,
                                         struct avl_node *old,
                                         struct avl_node *new,
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
    return avl_change_child(parent, node, right_rotate(node), root);

  } else if (balance < -threshold) {
    /* right-left heavy? */
    if (deviation(node->right) > 0) {
      node->right = right_rotate(node->right);
    }
    return avl_change_child(parent, node, left_rotate(node), root);
  }
  node->height = avl_height(node);
  return node;
}

static void restore_avl_properties(struct avl_node **root,
                                   struct avl_node *node) {
  size_t height;

  while (node) {
    height = node->height;
    node = rebalance(root, node);
    /* Rebalancing didn't affect the height of the tree */
    if (node->height == height) {
      break;
    }
    node = node->parent;
  }
}

static void avl_free_rec(avl_tree *avl, struct avl_node *node) {
  if (!node) {
    return;
  }
  avl_free_rec(avl, node->left);
  avl_free_rec(avl, node->right);
  avl->destroy(node);
}

avl_tree *avl_create(cmp_avl_nodes_fn cmp, destroy_avl_node_fn destroy) {
  assert(cmp != NULL);

  avl_tree *avl = yu_allocate(sizeof(*avl));
  if (!avl) {
    return NULL;
  }

  avl->cmp = cmp;
  avl->destroy = destroy;
  avl->root = NULL;
  avl->size = 0;
  return avl;
}

void avl_destroy(avl_tree *avl) {
  if (!avl) {
    return;
  }
  if (avl->destroy) {
    avl_free_rec(avl, avl->root);
  }
  yu_free(avl);
}

void avl_replace_node(struct avl_node *victim, struct avl_node *new,
                      struct avl_node **root) {
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

struct avl_node *remove_node(avl_tree *avl, struct avl_node **link,
                             struct avl_node *node) {
  struct avl_node *new;
  struct avl_node *victim = node;
  avl->size--;

  /* Does the node have two children? */
  if (node->left && node->right) {
    link = &node->right;
    while ((*link)->left) {
      link = &(*link)->left;
    }

    new = *link;
    *link = (*link)->right;

    node = new->parent == victim ? new : new->parent;
    avl_replace_node(victim, new, &avl->root);
  } else {
    *link = node->left ? node->left : node->right;
    node = node->parent;
  }
  if (*link) {
    (*link)->parent = node;
  }
  if (avl->destroy) {
    avl->destroy(victim);
  }
  return node;
}

void avl_remove(avl_tree *avl, const struct avl_node *query) {
  assert(avl != NULL);

  struct avl_node **link = &avl->root;
  struct avl_node *node = NULL;

  while (*link) {
    int c = avl->cmp(query, *link);
    if (c < 0) {
      link = &(*link)->left;
    } else if (c > 0) {
      link = &(*link)->right;
    } else { /* Key found */
      node = remove_node(avl, link, *link);
      break;
    }
  }

  restore_avl_properties(&avl->root, node);
}

bool avl_insert(avl_tree *avl, struct avl_node *node) {
  assert(avl != NULL);

  struct avl_node **link = &avl->root;
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
  *link = avl_node_init(node, parent);
  restore_avl_properties(&avl->root, parent);
  return true;
}

struct avl_node *avl_find(avl_tree *avl, const struct avl_node *query) {
  assert(avl != NULL);

  struct avl_node *node = avl->root;
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
  return avl->root;
}

struct avl_node *avl_first(avl_tree *avl) {
  assert(avl != NULL);

  struct avl_node *node = avl->root;
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

  struct avl_node *node = avl->root;
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
