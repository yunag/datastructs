#include "datastructs/trie.h"
#include "datastructs/utils.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASCII_LENGTH 256

struct trie {
  struct trie_node *root;
  size_t depth;
};

struct trie_node {
  struct trie_node **child;
  uint32_t child_count;
  bool word;
};

static struct trie_node *create_trie_node(void) {
  struct trie_node *node = malloc(sizeof(*node));
  if (node == NULL) {
    fprintf(stderr, "Failed to allocate memory for trie node\n");
    return NULL;
  }
  node->child = calloc(ASCII_LENGTH, sizeof(struct trie_node *));
  if (node->child == NULL) {
    fprintf(stderr, "Failed to allocate memory for trie node\n");
    free(node);
    return NULL;
  }
  node->word = false;
  node->child_count = 0;
  return node;
}

static void free_trie_node(struct trie_node *node) {
  assert(node != NULL);

  free(node->child);
  free(node);
}

trie *trie_create(void) {
  trie *trie = malloc(sizeof(*trie));
  if (trie == NULL) {
    fprintf(stderr, "Failed to allocate memory for Trie\n");
    return NULL;
  }
  trie->depth = 0;
  trie->root = create_trie_node();
  return trie;
}

void trie_insert(trie *trie, char *word) {
  assert(trie != NULL && word != NULL);

  struct trie_node *root = trie->root;
  size_t i = 0;
  for (; word[i] != '\0'; ++i) {
    size_t idx = word[i];
    if (root->child[idx] == NULL) {
      root->child_count++;
      root->child[idx] = create_trie_node();
    }
    root = root->child[idx];
  }
  trie->depth = yu_max(i, trie->depth);
  root->word = true;
}

bool trie_search(trie *trie, char *word) {
  assert(trie != NULL && word != NULL);

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

bool trie_starts_with(trie *trie, char *prefix) {
  assert(trie != NULL && prefix != NULL);

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

bool trie_remove_rec(struct trie_node *node, char *word, bool *deleted) {
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
    free_trie_node(*child);
    *child = NULL;
    --(node->child_count);
  }

  return node->child_count == 0 && !node->word;
}

bool trie_remove(trie *trie, char *word) {
  assert(trie != NULL && word != NULL);

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
  free_trie_node(node);
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

void trie_free(trie *trie) {
  assert(trie != NULL);

  trie_free_rec(trie->root);
  free(trie);
}
