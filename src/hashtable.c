#include "datastructs/functions.h"
#include "datastructs/hash_table.h"
#include "datastructs/memory.h"
#include "datastructs/types.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define GET_BUCKET(htable, key) ((htable)->hash((key)) % (htable)->capacity)

struct hash_table {
  struct hash_entry **buckets; /* Buckets to store pointers to hash entrys */
  struct hash_entry head;      /* Head of `global` linked list */

  hash_fn hash;     /* Hash function */
  cmp_fn cmp;       /* Compare two keys */
  free_fn free_key; /* Function to free a key */
  free_fn free_val; /* Function to free a value */

  size_t size;     /* Number of elements*/
  size_t capacity; /* Capacity of the table */
};

uint64_t hash_bern(const void *key, size_t size) {
  const unsigned char *bytes = key;
  uint64_t hash = 5381;
  for (size_t i = 0; i < size; ++i) {
    hash = (hash << 5) + hash + bytes[i];
  }
  return hash;
}

static void hentry_destroy(hash_table *htable, struct hash_entry *hentry) {
  htable->free_key(hentry->key);
  htable->free_val(hentry->val);
  free(hentry);
}

static struct hash_entry *hentry_create(hash_table *htable, void *key,
                                        void *val) {
  struct hash_entry *entry = yu_allocate(sizeof(*entry));
  if (!entry) {
    return NULL;
  }
  entry->key = key;
  entry->val = val;
  return entry;
}

static bool rehash(hash_table *htable, size_t newsize) {
  struct hash_entry **nbuckets = yu_calloc(newsize, sizeof(*nbuckets));
  if (!nbuckets) {
    return false;
  }
  free(htable->buckets);
  htable->buckets = nbuckets;
  htable->capacity = newsize;

  struct hash_entry *walk = htable->head.ll_next;
  while (walk != &htable->head) {
    uint64_t bct = GET_BUCKET(htable, walk->key);

    walk->next = htable->buckets[bct];
    htable->buckets[bct] = walk;

    walk = walk->ll_next;
  }

  return true;
}

hash_table *htable_create(size_t capacity, hash_fn hash, cmp_fn cmp_key,
                          free_fn free_key, free_fn free_value) {
  assert(capacity > 0);
  assert(hash != NULL);
  assert(cmp_key != NULL);

  hash_table *htable = yu_allocate(sizeof(*htable));
  if (!htable) {
    return NULL;
  }
  htable->buckets = yu_calloc(capacity, sizeof(*htable->buckets));
  if (!htable->buckets) {
    free(htable);
    return NULL;
  }
  htable->cmp = cmp_key;
  htable->hash = hash;
  htable->free_key = free_key ? free_key : free_placeholder;
  htable->free_val = free_value ? free_value : free_placeholder;
  htable->head.ll_next = htable->head.ll_prev = &htable->head;
  htable->head.key = NULL;
  htable->size = 0;
  htable->capacity = capacity;
  return htable;
}

void htable_destroy(hash_table *htable) {
  if (!htable) {
    return;
  }
  struct hash_entry *walk = htable->head.ll_next;
  while (walk != &htable->head) {
    struct hash_entry *tmp = walk;
    walk = walk->ll_next;
    hentry_destroy(htable, tmp);
  }
  free(htable->buckets);
  free(htable);
}

static struct hash_entry **lookup(hash_table *htable, const void *key,
                                  uint64_t bucket) {
  struct hash_entry **walk = &htable->buckets[bucket];
  while (*walk && htable->cmp((*walk)->key, key)) {
    walk = &(*walk)->next;
  }
  return walk;
}

bool htable_insert(hash_table *htable, void *key, void *val) {
  assert(htable != NULL);
  assert(key != NULL);

  if (htable->capacity == htable->size && !rehash(htable, htable->size * 2)) {
    return false;
  }
  uint64_t bct = GET_BUCKET(htable, key);
  struct hash_entry *entry = *lookup(htable, key, bct);
  if (entry) { /* The key already exists in the hash table */
    htable->free_val(entry->val);
    htable->free_key(key);
    entry->val = val;
    return true;
  }
  entry = hentry_create(htable, key, val);
  if (!entry) {
    htable->free_key(key);
    htable->free_val(val);
    return false;
  }
  struct hash_entry *tail = htable->head.ll_prev;
  entry->ll_prev = tail;
  entry->ll_next = tail->ll_next;
  tail->ll_next->ll_prev = entry;
  tail->ll_next = entry;

  entry->next = htable->buckets[bct];
  htable->buckets[bct] = entry;
  htable->size++;
  return true;
}

void *htable_lookup(hash_table *htable, const void *key) {
  assert(htable != NULL);
  assert(key != NULL);

  struct hash_entry *entry = *lookup(htable, key, GET_BUCKET(htable, key));
  return entry ? entry->val : NULL;
}

bool htable_contains(hash_table *htable, const void *key) {
  assert(htable != NULL);
  assert(key != NULL);

  struct hash_entry *entry = *lookup(htable, key, GET_BUCKET(htable, key));
  return entry;
}

bool htable_remove(hash_table *htable, const void *key) {
  assert(htable != NULL);
  assert(key != NULL);

  struct hash_entry **fentry = lookup(htable, key, GET_BUCKET(htable, key));
  if (!*fentry) {
    return false;
  }
  struct hash_entry *entry = *fentry;
  entry->ll_prev->ll_next = entry->ll_next;
  entry->ll_next->ll_prev = entry->ll_prev;
  *fentry = (*fentry)->next;

  hentry_destroy(htable, entry);
  htable->size--;
  return true;
}

size_t htable_size(hash_table *htable) {
  assert(htable != NULL);
  return htable->size;
}

hash_entry *ht_first(hash_table *htable) {
  assert(htable != NULL);
  return htable->head.ll_next;
}

hash_entry *ht_last(hash_table *htable) {
  assert(htable != NULL);
  return htable->head.ll_prev;
}

hash_entry *ht_next(hash_entry *entry) {
  assert(entry != NULL);
  entry = entry->ll_next;
  if (entry->key == NULL) {
    entry = NULL;
  }
  return entry;
}

hash_entry *ht_prev(hash_entry *entry) {
  assert(entry != NULL);
  entry = entry->ll_prev;
  if (entry->key == NULL) {
    entry = NULL;
  }
  return entry;
}
