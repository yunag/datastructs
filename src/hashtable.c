#include "datastructs/hash_table.h"
#include "datastructs/utils.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct helem {
  void *key;          /* Pointer to key copy */
  void *val;          /* Pointer to value copy */
  struct helem *next; /* Pointer to next struct */
};

struct hash_table {
  struct helem **table; /* Table to store pointers to hash elements */
  size_t ksize;         /* Size of a key */
  size_t vsize;         /* Size of a value */
  size_t size;          /* Number elements in the table */
  size_t capacity;      /* Number of elements in a table */
};

static size_t hash(hash_table *htable, const void *key) {
  const char *bytes = key;
  size_t hash = 5381;
  for (size_t i = 0; i < htable->ksize; ++i) {
    hash = (hash << 5) + hash + bytes[i];
  }
  return hash % htable->capacity;
}

hash_table *htable_create(size_t table_size, size_t key_size,
                          size_t value_size) {
  assert(table_size > 0 && key_size > 0 && value_size > 0);
  hash_table *htable = malloc(sizeof(*htable));
  if (htable == NULL) {
    yu_log_error("Failed to allocate memory for hash table");
    return NULL;
  }
  htable->table = calloc(table_size, sizeof(struct helem *));
  if (htable->table == NULL) {
    free(htable);
    yu_log_error("Failed to allocate memory for table");
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
      helem = helem->next;
      /* We store all the memory for key and value in `key` */
      free(to_free->key);
      free(to_free);
    }
  }
  free(htable->table);
  free(htable);
}

static struct helem **lookup(hash_table *htable, const void *key, size_t idx) {
  struct helem **walk = &htable->table[idx];
  while (*walk != NULL && memcmp((*walk)->key, key, htable->ksize)) {
    walk = &(*walk)->next;
  }
  return walk;
}

void htable_insert(hash_table *htable, const void *key, const void *val) {
  assert(htable != NULL && key != NULL && val != NULL);
  size_t idx = hash(htable, key);
  struct helem *entry = *lookup(htable, key, idx);
  if (entry != NULL) { /* The key already exists in the hash table */
    memcpy(entry->val, val, htable->vsize);
    return;
  }
  entry = malloc(sizeof(struct helem));
  if (entry == NULL) {
    yu_log_error("Failed to allocate memory for element of hash table");
    return;
  }
  char *keyval = malloc(htable->ksize + htable->vsize);
  if (keyval == NULL) {
    free(entry);
    yu_log_error("Failed to allocate memory for key and value");
    return;
  }
  entry->key = &keyval[0];
  entry->val = &keyval[htable->ksize];

  memcpy(entry->key, key, htable->ksize);
  memcpy(entry->val, val, htable->vsize);

  entry->next = htable->table[idx];
  htable->table[idx] = entry;
  htable->size++;
}

void *htable_lookup(hash_table *htable, const void *key) {
  assert(htable != NULL && key != NULL);
  struct helem *entry = *lookup(htable, key, hash(htable, key));
  return entry != NULL ? entry->val : NULL;
}

void htable_remove(hash_table *htable, const void *key) {
  assert(htable != NULL && key != NULL);
  struct helem **entry = lookup(htable, key, hash(htable, key));
  if (*entry != NULL) {
    struct helem *tmp = *entry;
    *entry = (*entry)->next;
    free(tmp);
  }
}

size_t htable_size(hash_table *hash_table) {
  assert(hash_table != NULL);
  return hash_table->size;
}

size_t htable_ksize(hash_table *hash_table) {
  assert(hash_table != NULL);
  return hash_table->ksize;
}

size_t htable_vsize(hash_table *hash_table) {
  assert(hash_table != NULL);
  return hash_table->vsize;
}
