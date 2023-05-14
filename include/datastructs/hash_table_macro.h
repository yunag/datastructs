#include "datastructs/hash_table.h"
#include "datastructs/macros.h"
#include "datastructs/types.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define GET_BUCKET(ht, key) ((ht)->hash(&(key), sizeof(key)) % (ht)->capacity)

#define HANDLE_EQUAL_KEYS                                                      \
  do {                                                                         \
    ht->vfree(&entry->val);                                                    \
    entry->val = val;                                                          \
  } while (0)

#define HTABLE_INSERT_BODY(handle_equal_keys, ret_on_equal_keys, postfix)      \
  do {                                                                         \
    assert(ht != NULL);                                                        \
                                                                               \
    if (ht->capacity == ht->size && !rehash##postfix(ht, ht->size * 2)) {      \
      return false;                                                            \
    }                                                                          \
    uint64_t bct = GET_BUCKET(ht, key);                                        \
    struct hash_entry##postfix *entry = *lookup##postfix(ht, key, bct);        \
    if (entry) { /* The key already exists in the hash table */                \
      handle_equal_keys;                                                       \
      return ret_on_equal_keys;                                                \
    }                                                                          \
    entry = hentry_create##postfix(ht, key, val);                              \
    if (!entry) {                                                              \
      YU_LOG_ERROR("Failed to allocate memory for hash entry");                \
      return false;                                                            \
    }                                                                          \
    struct hash_entry##postfix *tail = ht->head.lprev;                         \
    entry->lprev = tail;                                                       \
    entry->lnext = tail->lnext;                                                \
    tail->lnext->lprev = entry;                                                \
    tail->lnext = entry;                                                       \
                                                                               \
    entry->next = ht->buckets[bct];                                            \
    ht->buckets[bct] = entry;                                                  \
    ht->size++;                                                                \
    return true;                                                               \
  } while (0)

#define HASH_TABLE_INIT(Key_Type, Val_Type, postfix)                           \
  struct hash_entry##postfix {                                                 \
    Key_Type key; /* Pointer to key copy */                                    \
    Val_Type val; /* Pointer to value copy */                                  \
    struct hash_entry##postfix *lnext;                                         \
    struct hash_entry##postfix *lprev;                                         \
    struct hash_entry##postfix *next; /* Pointer to next struct */             \
  };                                                                           \
                                                                               \
  typedef struct hash_table##postfix {                                         \
    struct hash_entry##postfix *                                               \
        *buckets; /* Buckets to store pointers to hash entrys */               \
    struct hash_entry##postfix head; /* Head of `global` linked list */        \
    hash_fn hash;                    /* Hash function */                       \
    cmp_key_fn cmp_key;              /* Compare two keys */                    \
    free_fn kfree;                   /* Function to free a key */              \
    free_fn vfree;                   /* Function to free a value */            \
    size_t size;                     /* Number of elements*/                   \
    size_t capacity;                 /* Capacity of the table */               \
  } hash_table##postfix;                                                       \
                                                                               \
  void hentry_free##postfix(hash_table##postfix *ht,                           \
                            struct hash_entry##postfix *hentry) {              \
    ht->kfree(&hentry->key);                                                   \
    ht->vfree(&hentry->val);                                                   \
    free(hentry);                                                              \
  }                                                                            \
                                                                               \
  struct hash_entry##postfix *hentry_create##postfix(                          \
      hash_table##postfix *ht, Key_Type key, Val_Type val) {                   \
    struct hash_entry##postfix *entry = malloc(sizeof(*entry));                \
    if (!entry) {                                                              \
      return NULL;                                                             \
    }                                                                          \
    entry->key = key;                                                          \
    entry->val = val;                                                          \
    return entry;                                                              \
  }                                                                            \
                                                                               \
  bool rehash##postfix(hash_table##postfix *ht, size_t newsize) {              \
    struct hash_entry##postfix **nbuckets =                                    \
        calloc(newsize, sizeof(*nbuckets));                                    \
    if (!nbuckets) {                                                           \
      YU_LOG_ERROR("Failed to resize the hash table to %zu", newsize);         \
      return false;                                                            \
    }                                                                          \
    free(ht->buckets);                                                         \
    ht->buckets = nbuckets;                                                    \
    ht->capacity = newsize;                                                    \
                                                                               \
    struct hash_entry##postfix *walk = ht->head.lnext;                         \
    while (walk != &ht->head) {                                                \
      uint64_t bct = GET_BUCKET(ht, walk->key);                                \
                                                                               \
      walk->next = ht->buckets[bct];                                           \
      ht->buckets[bct] = walk;                                                 \
                                                                               \
      walk = walk->lnext;                                                      \
    }                                                                          \
                                                                               \
    return true;                                                               \
  }                                                                            \
                                                                               \
  hash_table##postfix *ht_create##postfix(                                     \
      size_t capacity, hash_fn user_hash, cmp_key_fn cmp_key,                  \
      free_fn key_free, free_fn value_free) {                                  \
    assert(capacity > 0);                                                      \
                                                                               \
    hash_table##postfix *ht = malloc(sizeof(*ht));                             \
    if (!ht) {                                                                 \
      YU_LOG_ERROR("Failed to allocate memory for hash table");                \
      return NULL;                                                             \
    }                                                                          \
    ht->buckets = calloc(capacity, sizeof(*ht->buckets));                      \
    if (!ht->buckets) {                                                        \
      free(ht);                                                                \
      YU_LOG_ERROR("Failed to allocate memory for table");                     \
      return NULL;                                                             \
    }                                                                          \
    ht->cmp_key = cmp_key ? cmp_key : memcmp;                                  \
    ht->hash = user_hash ? user_hash : hash_fnv1a;                             \
    ht->vfree = value_free ? value_free : free_placeholder;                    \
    ht->kfree = key_free ? key_free : free_placeholder;                        \
    ht->head.lnext = ht->head.lprev = &ht->head;                               \
    ht->size = 0;                                                              \
    ht->capacity = capacity;                                                   \
    return ht;                                                                 \
  }                                                                            \
                                                                               \
  void ht_destroy##postfix(hash_table##postfix *ht) {                          \
    assert(ht != NULL);                                                        \
    struct hash_entry##postfix *walk = ht->head.lnext;                         \
    while (walk != &ht->head) {                                                \
      struct hash_entry##postfix *tmp = walk;                                  \
      walk = walk->lnext;                                                      \
      hentry_free##postfix(ht, tmp);                                           \
    }                                                                          \
    free(ht->buckets);                                                         \
    free(ht);                                                                  \
  }                                                                            \
                                                                               \
  struct hash_entry##postfix **lookup##postfix(                                \
      hash_table##postfix *ht, Key_Type key, uint64_t bucket) {                \
    struct hash_entry##postfix **walk = &ht->buckets[bucket];                  \
    while (*walk && (*walk)->key != key) {                                     \
      walk = &(*walk)->next;                                                   \
    }                                                                          \
    return walk;                                                               \
  }                                                                            \
                                                                               \
  bool ht_insert##postfix(hash_table##postfix *ht, Key_Type key,               \
                          Val_Type val) {                                      \
    HT_INSERT_BODY(HANDLE_EQUAL_KEYS, true, postfix);                          \
  }                                                                            \
                                                                               \
  bool ht_try_insert##postfix(hash_table##postfix *ht, Key_Type key,           \
                              Val_Type val) {                                  \
    HTABLE_INSERT_BODY(, false, postfix);                                      \
  }                                                                            \
                                                                               \
  Val_Type *ht_lookup##postfix(hash_table##postfix *ht, Key_Type key) {        \
    assert(ht != NULL);                                                        \
                                                                               \
    struct hash_entry##postfix *entry =                                        \
        *lookup##postfix(ht, key, GET_BUCKET(ht, key));                        \
    return entry ? &entry->val : NULL;                                         \
  }                                                                            \
                                                                               \
  bool ht_contains##postfix(hash_table##postfix *ht, Key_Type key) {           \
    assert(ht != NULL);                                                        \
                                                                               \
    struct hash_entry##postfix *entry =                                        \
        *lookup##postfix(ht, key, GET_BUCKET(ht, key));                        \
    return entry;                                                              \
  }                                                                            \
                                                                               \
  void ht_remove##postfix(hash_table##postfix *ht, Key_Type key) {             \
    assert(ht != NULL);                                                        \
                                                                               \
    struct hash_entry##postfix **fentry =                                      \
        lookup##postfix(ht, key, GET_BUCKET(ht, key));                         \
    if (!*fentry) {                                                            \
      return;                                                                  \
    }                                                                          \
    struct hash_entry##postfix *entry = *fentry;                               \
    entry->lprev->lnext = entry->lnext;                                        \
    entry->lnext->lprev = entry->lprev;                                        \
    *fentry = (*fentry)->next;                                                 \
                                                                               \
    hentry_free##postfix(ht, entry);                                           \
    ht->size--;                                                                \
  }\
