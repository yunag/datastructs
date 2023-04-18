#include "datastructs/hash_table.h"
#include "datastructs/utils.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define GET_BUCKET(htable, key)                                                \
  ((htable)->hash((key), (htable)->ksize) % (htable)->capacity)

struct hash_entry {
  void *key; /* Pointer to key copy */
  void *val; /* Pointer to value copy */
  struct hash_entry *lnext;
  struct hash_entry *lprev;
  struct hash_entry *next; /* Pointer to next struct */
};

struct hash_table {
  struct hash_entry **buckets; /* Buckets to store pointers to hash entrys */
  struct hash_entry head;      /* Head of `global` linked list */
  hash_fn hash;                /* Hash function */
  free_fn vfree;               /* Function to free a value */
  size_t ksize;                /* Size of a key */
  size_t vsize;                /* Size of a value */
  size_t size;                 /* Number of elements*/
  size_t capacity;             /* Capacity of the table */
};

static uint64_t hash_bern(const void *key, size_t size) {
  const unsigned char *bytes = key;
  uint64_t hash = 5381;
  for (size_t i = 0; i < size; ++i) {
    hash = (hash << 5) + hash + bytes[i];
  }
  return hash;
}

#define FNV_PRIME 0x100000001b3
#define FNV_OFFSET 0xcbf29ce484222325UL
static uint64_t hash_fnv1a(const void *key, size_t size) {
  const unsigned char *bytes = key;
  uint64_t hashv = FNV_OFFSET;
  for (size_t i = 0; i < size; ++i) {
    hashv ^= bytes[i];
    hashv *= FNV_PRIME;
  }
  return hashv;
}

static struct hash_entry *hash_entry_create(hash_table *htable, const void *key,
                                            const void *val) {}

static void hash_entry_free(struct hash_entry *hentry) {
  free(hentry->key);
  free(hentry->val);
  free(hentry);
}

static bool rehash(hash_table *htable, size_t newsize) {
  struct hash_entry **nbuckets = calloc(newsize, sizeof(struct hash_entry *));
  if (nbuckets == NULL) {
    YU_LOG_ERROR("Failed to resize the hash table to %zu", newsize);
    return false;
  }
  free(htable->buckets);
  htable->buckets = nbuckets;
  htable->capacity = newsize;

  struct hash_entry *walk = htable->head.lnext;
  while (walk != &htable->head) {
    uint64_t bct = GET_BUCKET(htable, walk->key);

    walk->next = htable->buckets[bct];
    htable->buckets[bct] = walk;

    walk = walk->lnext;
  }

  return true;
}

hash_table *htable_create(size_t capacity, size_t key_size, size_t value_size,
                          hash_fn user_hash, free_fn val_free) {
  assert(capacity > 0);
  assert(key_size > 0);

  hash_table *htable = malloc(sizeof(*htable));
  if (htable == NULL) {
    YU_LOG_ERROR("Failed to allocate memory for hash table");
    return NULL;
  }
  htable->buckets = calloc(capacity, sizeof(*htable->buckets));
  if (htable->buckets == NULL) {
    free(htable);
    YU_LOG_ERROR("Failed to allocate memory for table");
    return NULL;
  }
  htable->hash = user_hash ? user_hash : hash_fnv1a;
  htable->vfree = val_free ? val_free : free_placeholder;
  htable->head.lnext = htable->head.lprev = &htable->head;
  htable->size = 0;
  htable->capacity = capacity;
  htable->ksize = key_size;
  htable->vsize = value_size;
  return htable;
}

void htable_free(hash_table *htable) {
  assert(htable != NULL);
  struct hash_entry *walk = htable->head.lnext;
  while (walk != &htable->head) {
    struct hash_entry *tmp = walk;
    walk = walk->lnext;

    htable->vfree(tmp->val);
    hash_entry_free(tmp);
  }
  free(htable->buckets);
  free(htable);
}

static struct hash_entry **lookup(hash_table *htable, const void *key,
                                  uint64_t bucket) {
  struct hash_entry **walk = &htable->buckets[bucket];
  while (*walk != NULL && memcmp((*walk)->key, key, htable->ksize)) {
    walk = &(*walk)->next;
  }
  return walk;
}

void htable_insert(hash_table *htable, const void *key, const void *val) {
  assert(htable != NULL);
  assert(key != NULL);
  if (htable->capacity == htable->size && !rehash(htable, htable->size * 2)) {
    return;
  }

  uint64_t bct = GET_BUCKET(htable, key);
  struct hash_entry *entry = *lookup(htable, key, bct);
  if (entry != NULL) { /* The key already exists in the hash table */
    memcpy(entry->val, val, htable->vsize);
    return;
  }
  entry = malloc(sizeof(*entry));
  entry->key = malloc(htable->ksize);
  entry->val = malloc(htable->vsize);

  if (!entry || !entry->key || !entry->val) {
    hash_entry_free(entry);
    YU_LOG_ERROR("Failed to allocate memory for hash entry");
    return;
  }
  struct hash_entry *tail = htable->head.lprev;
  entry->lprev = tail;
  entry->lnext = tail->lnext;
  tail->lnext->lprev = entry;
  tail->lnext = entry;

  memcpy(entry->key, key, htable->ksize);
  if (val) {
    memcpy(entry->val, val, htable->vsize);
  }

  entry->next = htable->buckets[bct];
  htable->buckets[bct] = entry;
  htable->size++;
}

void *htable_lookup(hash_table *htable, const void *key) {
  assert(htable != NULL);
  assert(key != NULL);

  struct hash_entry *entry = *lookup(htable, key, GET_BUCKET(htable, key));
  return entry != NULL ? entry->val : NULL;
}

void htable_remove(hash_table *htable, const void *key) {
  assert(htable != NULL);
  assert(key != NULL);

  struct hash_entry **fentry = lookup(htable, key, GET_BUCKET(htable, key));
  if (*fentry == NULL) {
    return;
  }
  struct hash_entry *entry = *fentry;
  entry->lprev->lnext = entry->lnext;
  entry->lnext->lprev = entry->lprev;
  *fentry = (*fentry)->next;

  htable->vfree(entry->val);
  hash_entry_free(entry);
  htable->size--;
}

size_t htable_size(hash_table *htable) {
  assert(htable != NULL);
  return htable->size;
}

size_t htable_ksize(hash_table *htable) {
  assert(htable != NULL);
  return htable->ksize;
}

size_t htable_vsize(hash_table *htable) {
  assert(htable != NULL);
  return htable->vsize;
}
