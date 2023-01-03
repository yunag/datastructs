#ifndef YU_TRIE_H
#define YU_TRIE_H

#include "types.h"

#include <stdbool.h>

#define TRIE_REMOVED 0
#define TRIE_REMOVE_FAIL -1

typedef struct trie trie;

trie *trie_create(void);

void trie_free(trie *trie);

void trie_insert(trie *trie, char *word);

bool trie_search(trie *trie, char *word);

bool trie_remove(trie *trie, char *word);

bool trie_starts_with(trie *trie, char *prefix);

void trie_print(trie *trie);

#endif // !YU_TRIE_H
