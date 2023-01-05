#ifndef YU_TRIE_H
#define YU_TRIE_H

#include "types.h"
#include <stdbool.h>

typedef struct trie trie;

trie *trie_create(void);
void trie_free(trie *trie);
void trie_insert(trie *trie, const char *word);
bool trie_search(trie *trie, const char *word);
bool trie_remove(trie *trie, const char *word);
bool trie_starts_with(trie *trie, const char *prefix);
void trie_print(trie *trie);

#endif // !YU_TRIE_H
