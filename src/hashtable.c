#include "datastructs/hashtable.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct helem {
  void *key;          /* Pointer to key copy */
  void *val;          /* Pointer to value copy */
  struct helem *prev; /* Pointer to previous struct */
};

struct hash_table {
  struct helem **table; /* Table to store pointers to hash elements */
  size_t ksize;         /* Size of a key */
  size_t vsize;         /* Size of a value */
  size_t size;          /* Number elements in the table */
  size_t capacity;      /* Number of elements in a table */
};

static size_t hash(hash_table *htable, const void *key) {
  assert(htable != NULL && key != NULL);
  const uint8_t *bytes = key;
  size_t sum = 0;
  for (size_t i = 0; i < htable->ksize; ++i) {
    sum += bytes[i];
  }
  return sum % htable->capacity;
}

hash_table *htable_create(size_t table_size, size_t key_size,
                          size_t value_size) {
  assert(table_size > 0 && key_size > 0 && value_size > 0);
  hash_table *htable = malloc(sizeof(*htable));
  if (htable == NULL) {
    fprintf(stderr, "Failed to allocate memory for hash table.\n");
    return NULL;
  }
  htable->table = calloc(table_size, sizeof(struct helem *));
  if (htable->table == NULL) {
    free(htable);
    fprintf(stderr, "Failed to allocate memory for table.\n");
    return NULL;
  }
  htable->size = 0;
  htable->capacity = table_size;
  htable->ksize = key_size;
  htable->vsize = value_size;
  return htable;
}

void htable_free(hash_table *htable) {
  assert(htable != NULL);
  for (size_t i = 0; i < htable->capacity; i++) {
    struct helem *helem = htable->table[i];
    while (helem != NULL) {
      struct helem *to_free = helem;
      helem = helem->prev;
      /* We store all the memory for key and value in `key` */
      free(to_free->key);
      free(to_free);
    }
  }
  free(htable->table);
  free(htable);
}

static void *lookup(hash_table *htable, const void *key, size_t idx) {
  struct helem *tmp = htable->table[idx];
  while (tmp != NULL && memcmp(tmp->key, key, htable->ksize)) {
    tmp = tmp->prev;
  }
  return tmp != NULL ? tmp->val : tmp;
}

void htable_insert(hash_table *htable, const void *key, const void *val) {
  assert(htable != NULL && key != NULL && val != NULL);
  void *exist = lookup(htable, key, hash(htable, key));
  if (exist != NULL) { /* The key already exists in the hash table */
    memcpy(exist, val, htable->vsize);
    return;
  }
  struct helem *tmp = malloc(sizeof(struct helem));
  if (tmp == NULL) {
    fprintf(stderr, "Failed to allocate memory for element of hash table.\n");
    return;
  }
  uint8_t *keyval = malloc(htable->ksize + htable->vsize);
  if (keyval == NULL) {
    free(tmp);
    fprintf(stderr, "Failed to allocate memory for key and value.\n");
    return;
  }
  size_t idx = hash(htable, key);
  tmp->key = &keyval[0];
  tmp->val = &keyval[htable->ksize];

  memcpy(tmp->key, key, htable->ksize);
  memcpy(tmp->val, val, htable->vsize);

  tmp->prev = htable->table[idx];
  htable->table[idx] = tmp;
  htable->size++;
}

void *htable_lookup(hash_table *htable, const void *key) {
  assert(htable != NULL && key != NULL);
  return lookup(htable, key, hash(htable, key));
}

inline size_t htable_size(hash_table *hash_table) {
  assert(hash_table != NULL);
  return hash_table->size;
}

inline size_t htable_ksize(hash_table *hash_table) {
  assert(hash_table != NULL);
  return hash_table->ksize;
}

inline size_t htable_vsize(hash_table *hash_table) {
  assert(hash_table != NULL);
  return hash_table->vsize;
}
