#include "datastructs/functions.h"
#include "datastructs/hash_table.h"
#include "datastructs/macros.h"
#include "datastructs/types.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define GET_BUCKET(htable, key) ((htable)->hash((key)) % (htable)->capacity)

struct hash_entry {
  void *key; /* Pointer to key */
  void *val; /* Pointer to value */
  struct hash_entry *gnext;
  struct hash_entry *gprev;

  struct hash_entry *next; /* Pointer to next struct */
};

struct hash_table {
  struct hash_entry **buckets; /* Buckets to store pointers to hash entrys */
  struct hash_entry head;      /* Head of `global` linked list */
  hash_fn hash;                /* Hash function */
  cmp_key_fn cmp_key;          /* Compare two keys */
  free_fn kfree;               /* Function to free a key */
  free_fn vfree;               /* Function to free a value */
  size_t size;                 /* Number of elements*/
  size_t capacity;             /* Capacity of the table */
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
  htable->kfree(hentry->key);
  htable->vfree(hentry->val);
  free(hentry);
}

static struct hash_entry *hentry_create(hash_table *htable, void *key,
                                        void *val) {
  struct hash_entry *entry = malloc(sizeof(*entry));
  if (!entry) {
    YU_LOG_ERROR("Failed to allocate memory for hash entry");
    return NULL;
  }
  entry->key = key;
  entry->val = val;
  return entry;
}

static bool rehash(hash_table *htable, size_t newsize) {
  struct hash_entry **nbuckets = calloc(newsize, sizeof(*nbuckets));
  if (!nbuckets) {
    YU_LOG_ERROR("Failed to resize the hash table to %zu", newsize);
    return false;
  }
  free(htable->buckets);
  htable->buckets = nbuckets;
  htable->capacity = newsize;

  struct hash_entry *walk = htable->head.gnext;
  while (walk != &htable->head) {
    uint64_t bct = GET_BUCKET(htable, walk->key);

    walk->next = htable->buckets[bct];
    htable->buckets[bct] = walk;

    walk = walk->gnext;
  }

  return true;
}

hash_table *htable_create(size_t capacity, hash_fn hash, cmp_key_fn cmp_key,
                          free_fn free_key, free_fn free_value) {
  assert(capacity > 0);
  assert(hash != NULL);
  assert(cmp_key != NULL);

  hash_table *htable = malloc(sizeof(*htable));
  if (!htable) {
    YU_LOG_ERROR("Failed to allocate memory for hash table");
    return NULL;
  }
  htable->buckets = calloc(capacity, sizeof(*htable->buckets));
  if (!htable->buckets) {
    free(htable);
    YU_LOG_ERROR("Failed to allocate memory for table");
    return NULL;
  }
  htable->cmp_key = cmp_key;
  htable->hash = hash;
  htable->kfree = free_key ? free_key : free_placeholder;
  htable->vfree = free_value ? free_value : free_placeholder;
  htable->head.gnext = htable->head.gprev = &htable->head;
  htable->size = 0;
  htable->capacity = capacity;
  return htable;
}

void htable_destroy(hash_table *htable) {
  if (!htable) {
    return;
  }
  struct hash_entry *walk = htable->head.gnext;
  while (walk != &htable->head) {
    struct hash_entry *tmp = walk;
    walk = walk->gnext;
    hentry_destroy(htable, tmp);
  }
  free(htable->buckets);
  free(htable);
}

static struct hash_entry **lookup(hash_table *htable, const void *key,
                                  uint64_t bucket) {
  struct hash_entry **walk = &htable->buckets[bucket];
  while (*walk && !htable->cmp_key((*walk)->key, key)) {
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
    htable->vfree(entry->val);
    htable->kfree(key);
    entry->val = val;
    return true;
  }
  entry = hentry_create(htable, key, val);
  if (!entry) {
    htable->kfree(key);
    htable->vfree(val);
    return false;
  }
  struct hash_entry *tail = htable->head.gprev;
  entry->gprev = tail;
  entry->gnext = tail->gnext;
  tail->gnext->gprev = entry;
  tail->gnext = entry;

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
  entry->gprev->gnext = entry->gnext;
  entry->gnext->gprev = entry->gprev;
  *fentry = (*fentry)->next;

  hentry_destroy(htable, entry);
  htable->size--;
  return true;
}

size_t htable_size(hash_table *htable) {
  assert(htable != NULL);
  return htable->size;
}

ht_iterator *ht_begin(hash_table *htable) {
  assert(htable != NULL);
  return (ht_iterator *)htable->head.gnext;
}

ht_iterator *ht_end(hash_table *htable) {
  assert(htable != NULL);
  return (ht_iterator *)&htable->head;
}

ht_iterator *ht_next(ht_iterator *iterator) {
  assert(iterator != NULL);
  struct hash_entry *hentry = (struct hash_entry *)iterator;
  return (ht_iterator *)hentry->gnext;
}

struct key_value ht_get(ht_iterator *iterator) {
  assert(iterator != NULL);
  struct hash_entry *hentry = (struct hash_entry *)iterator;
  return (struct key_value){
      .key = hentry->key,
      .val = hentry->val,
  };
}
