#include "datastructs/avl_tree.h"
#include "datastructs/macros.h"

#include <assert.h>
#include <stdlib.h>

static const int threshold = 1;

static inline int avl_node_height(struct avl_node *node) {
  return !node ? 0 : node->height;
}

static inline int avl_deviation(struct avl_node *node) {
  return avl_node_height(node->left) - avl_node_height(node->right);
}

static inline int avl_height(struct avl_node *node) {
  return 1 + YU_MAX(avl_node_height(node->left), avl_node_height(node->right));
}

static struct avl_node *avl_left_rotate(struct avl_node *node) {
  struct avl_node *rnode = node->right;
  node->right = rnode->left;
  rnode->left = node;

  rnode->parent = node->parent;
  node->parent = rnode;
  if (node->right) {
    node->right->parent = node;
  }

  node->height = avl_height(node);
  rnode->height = avl_height(rnode);
  return rnode;
}

static struct avl_node *avl_right_rotate(struct avl_node *node) {
  struct avl_node *lnode = node->left;
  node->left = lnode->right;
  lnode->right = node;

  lnode->parent = node->parent;
  node->parent = lnode;
  if (node->left) {
    node->left->parent = node;
  }

  node->height = avl_height(node);
  lnode->height = avl_height(lnode);
  return lnode;
}

static void avl_change_child(struct avl_node *parent, struct avl_node *old,
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

static struct avl_node *avl_rebalance(struct avl_root *root,
                                      struct avl_node *node) {
  struct avl_node *parent = node->parent;
  struct avl_node *new = node;

  size_t height = node->height;
  int balance = avl_deviation(node);

  if (balance > threshold) {
    /* left-right heavy? */
    if (avl_deviation(node->left) < 0) {
      node->left = avl_left_rotate(node->left);
    }
    avl_change_child(parent, node, new = avl_right_rotate(node), root);

  } else if (balance < -threshold) {
    /* right-left heavy? */
    if (avl_deviation(node->right) > 0) {
      node->right = avl_right_rotate(node->right);
    }
    avl_change_child(parent, node, new = avl_left_rotate(node), root);

  } else {
    node->height = avl_height(node);
  }

  return height != new->height ? parent : NULL;
}

void avl_link_node(struct avl_node *node, struct avl_node *parent,
                   struct avl_node **link) {
  node->left = node->right = NULL;
  node->parent = parent;
  node->height = 1;

  *link = node;
}

void avl_restore_properties(struct avl_root *root, struct avl_node *node) {
  while (node) {
    node = avl_rebalance(root, node);
  }
}

void avl_init(avl_tree *avl, compare_avl_nodes_fun cmp) {
  assert(avl != NULL);
  assert(cmp != NULL);

  avl->cmp = cmp;
  avl->root.avl_node = NULL;
  avl->num_items = 0;
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

    new = node->parent != victim ? node->parent : node;

    avl_replace_node(victim, node, root);
  }

  if (*link) {
    (*link)->parent = new;
  }

  return new;
}

struct avl_node *avl_remove(avl_tree *avl, const struct avl_node *query) {
  assert(avl != NULL);

  struct avl_node **link = &avl->root.avl_node;
  struct avl_node *node = NULL;
  struct avl_node *removed = NULL;

  while (*link) {
    int c = avl->cmp(query, *link);
    if (c < 0) {
      link = &(*link)->left;
    } else if (c > 0) {
      link = &(*link)->right;
    } else {
      removed = *link;

      node = avl_remove_node(&avl->root, link);
      avl->num_items--;
      break;
    }
  }

  avl_restore_properties(&avl->root, node);

  return removed;
}

struct avl_node *avl_insert(avl_tree *avl, struct avl_node *node) {
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
      return parent;
    }
  }

  avl->num_items++;
  avl_link_node(node, parent, link);
  avl_restore_properties(&avl->root, parent);

  return NULL;
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

static struct avl_node *avl_left_deepest_node(const struct avl_node *node) {
  for (;;) {
    if (node->left) {
      node = node->left;
    } else if (node->right) {
      node = node->right;
    } else {
      return (struct avl_node *)node;
    }
  }
}

struct avl_node *avl_first_postorder(const struct avl_root *root) {
  if (!root->avl_node) {
    return NULL;
  }

  return avl_left_deepest_node(root->avl_node);
}

struct avl_node *avl_next_postorder(const struct avl_node *node) {
  assert(node != NULL);

  const struct avl_node *parent = node->parent;
  if (parent && parent->left == node && parent->right) {

    return avl_left_deepest_node(parent->right);
  } else {

    return (struct avl_node *)parent;
  }
}

struct avl_node *avl_first(const struct avl_root *root) {
  assert(root != NULL);

  struct avl_node *node = root->avl_node;
  if (!node) {
    return NULL;
  }

  while (node->left) {
    node = node->left;
  }

  return node;
}

struct avl_node *avl_last(const struct avl_root *root) {
  assert(root != NULL);

  struct avl_node *node = root->avl_node;
  if (!node) {
    return NULL;
  }

  while (node->right) {
    node = node->right;
  }

  return node;
}

struct avl_node *avl_next(const struct avl_node *node) {
  assert(node != NULL);

  struct avl_node *parent;
  if (node->right) {
    node = node->right;
    while (node->left) {
      node = node->left;
    }
    return (struct avl_node *)node;
  }

  while ((parent = node->parent) && node == parent->right) {
    node = parent;
  }

  return parent;
}

struct avl_node *avl_prev(const struct avl_node *node) {
  assert(node != NULL);

  struct avl_node *parent;
  if (node->left) {
    node = node->left;
    while (node->right) {
      node = node->right;
    }
    return (struct avl_node *)node;
  }

  while ((parent = node->parent) && node == parent->left) {
    node = parent;
  }

  return parent;
}
