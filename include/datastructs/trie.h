#ifndef YU_TRIE_H
#define YU_TRIE_H

#include "types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct trie trie;

trie *trie_create(void);
void trie_free(trie *trie);
void trie_insert(trie *trie, const char *word);
bool trie_search(trie *trie, const char *word);
bool trie_remove(trie *trie, const char *word);
bool trie_starts_with(trie *trie, const char *prefix);
void trie_print(trie *trie);

#ifdef __cplusplus
}
#endif

#endif // !YU_TRIE_H
