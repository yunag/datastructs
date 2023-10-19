#include "datastructs/trie.h"
#include "datastructs/macros.h"
#include "datastructs/memory.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASCII_LENGTH 256

struct trie {
  struct trie_node *root; /* Root node */
  size_t depth;           /* Maximum depth of the Trie */
};

struct trie_node {
  struct trie_node **child; /* Childs of the current node */
  uint32_t child_count;     /* Number of childs */
  bool word;                /* Is the current node an end of the word? */
};

static struct trie_node *trie_node_create(void) {
  struct trie_node *node = _yu_allocator.allocate(sizeof(*node));
  if (!node) {
    return NULL;
  }
  node->child = _yu_allocator.calloc(ASCII_LENGTH, sizeof(struct trie_node *));
  if (!node->child) {
    free(node);
    return NULL;
  }
  node->word = false;
  node->child_count = 0;
  return node;
}

static void trie_node_destroy(struct trie_node *node) {
  assert(node != NULL);
  free(node->child);
  free(node);
}

trie *trie_create(void) {
  trie *trie = _yu_allocator.allocate(sizeof(*trie));
  if (!trie) {
    return NULL;
  }
  trie->depth = 0;
  trie->root = trie_node_create();
  return trie;
}

void trie_insert(trie *trie, const char *word) {
  assert(trie != NULL);
  assert(word != NULL);

  struct trie_node *root = trie->root;
  size_t i = 0;
  for (; word[i] != '\0'; ++i) {
    size_t idx = word[i];
    if (root->child[idx] == NULL) {
      root->child_count++;
      root->child[idx] = trie_node_create();
    }
    root = root->child[idx];
  }
  trie->depth = YU_MAX(i, trie->depth);
  root->word = true;
}

bool trie_search(trie *trie, const char *word) {
  assert(trie != NULL);
  assert(word != NULL);

  struct trie_node *root = trie->root;
  for (size_t i = 0; word[i] != '\0'; ++i) {
    size_t idx = word[i];
    if (root->child[idx] == NULL) {
      return false;
    }
    root = root->child[idx];
  }
  return root->word;
}

bool trie_starts_with(trie *trie, const char *prefix) {
  assert(trie != NULL);
  assert(prefix != NULL);

  struct trie_node *root = trie->root;
  for (size_t i = 0; prefix[i] != '\0'; ++i) {
    size_t idx = prefix[i];
    if (root->child[idx] == NULL) {
      return false;
    }
    root = root->child[idx];
  }
  return true;
}

static bool trie_remove_rec(struct trie_node *node, const char *word,
                            bool *deleted) {
  if (*word == '\0') {
    *deleted = node->word;
    node->word = false;
    return node->child_count == 0;
  }

  struct trie_node **child = &node->child[(size_t)*word];
  if (*child == NULL) {
    return false;
  }

  if (trie_remove_rec(*child, &word[1], deleted)) {
    trie_node_destroy(*child);
    *child = NULL;
    --(node->child_count);
  }

  return node->child_count == 0 && !node->word;
}

bool trie_remove(trie *trie, const char *word) {
  assert(trie != NULL);
  assert(word != NULL);

  bool deleted = false;
  trie_remove_rec(trie->root, word, &deleted);
  return deleted;
}

static void trie_free_rec(struct trie_node *node) {
  for (size_t i = 0; i < ASCII_LENGTH; ++i) {
    if (node->child[i] != NULL) {
      trie_free_rec(node->child[i]);
    }
  }
  trie_node_destroy(node);
}

void trie_print_rec(struct trie_node *node, char *buffer, size_t *bufsize) {
  if (node->word) {
    printf("%s\n", buffer);
  }
  for (size_t i = 0; i < ASCII_LENGTH; ++i) {
    if (node->child[i] != NULL) {
      buffer[(*bufsize)++] = i;
      trie_print_rec(node->child[i], buffer, bufsize);
      (*bufsize)--;
    }
  }
}

void trie_print(trie *trie) {
  assert(trie != NULL);
  size_t bufsize = trie->depth + 1;
  char buffer[bufsize];
  trie_print_rec(trie->root, buffer, &bufsize);
}

void trie_destroy(trie *trie) {
  if (trie) {
    trie_free_rec(trie->root);
    free(trie);
  }
}
