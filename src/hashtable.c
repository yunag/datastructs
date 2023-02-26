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
  struct hnode **node;
};

struct hnode {
  struct hnode *next;
  struct helem *val;
};

struct hash_table {
  struct helem **table; /* Table to store pointers to hash elements */
  struct hnode head;
  struct hnode **tail;
  size_t ksize;    /* Size of a key */
  size_t vsize;    /* Size of a value */
  size_t size;     /* Number elements in the table */
  size_t capacity; /* Number of elements in a table */
};

static size_t hash(hash_table *htable, const void *key) {
  const char *bytes = key;
  size_t hash = 5381;
  for (size_t i = 0; i < htable->ksize; ++i) {
    hash = (hash << 5) + hash + bytes[i];
  }
  return hash % htable->capacity;
}

static void helem_free(struct helem *helem) {
  free(helem->key);
  free(helem->val);
  free(helem);
}

static struct hnode *hnode_create(struct hnode *next, struct helem *val) {
  struct hnode *node = malloc(sizeof(*node));
  if (node == NULL) {
    YU_LOG_ERROR("Failed to create hash node\n");
    return NULL;
  }
  node->next = next;
  node->val = val;
  return node;
}

static void hnode_free(struct hnode *node) { free(node); }

static bool rehash(hash_table *htable, size_t newsize) {
  struct helem **ntable = calloc(newsize, sizeof(struct helem *));
  if (ntable == NULL) {
    YU_LOG_ERROR("Failed to resize hash table\n");
    return false;
  }
  free(htable->table);
  htable->table = ntable;
  htable->capacity = newsize;

  struct hnode *head = htable->head.next;
  while (head != NULL) {
    struct helem *entry = head->val;
    size_t idx = hash(htable, entry->key);

    entry->next = htable->table[idx];
    htable->table[idx] = entry;

    head = head->next;
  }

  return true;
}

hash_table *htable_create(size_t table_size, size_t key_size,
                          size_t value_size) {
  assert(table_size > 0 && key_size > 0 && value_size > 0);
  hash_table *htable = malloc(sizeof(*htable));
  if (htable == NULL) {
    YU_LOG_ERROR("Failed to allocate memory for hash table\n");
    return NULL;
  }
  htable->table = calloc(table_size, sizeof(struct helem *));
  if (htable->table == NULL) {
    free(htable);
    YU_LOG_ERROR("Failed to allocate memory for table\n");
    return NULL;
  }
  htable->head.next = NULL;
  htable->tail = &htable->head.next;
  htable->size = 0;
  htable->capacity = table_size;
  htable->ksize = key_size;
  htable->vsize = value_size;
  return htable;
}

void htable_free(hash_table *htable) {
  assert(htable != NULL);
  struct hnode *head = htable->head.next;
  while (head != NULL) {
    struct hnode *tmp = head;
    head = head->next;
    helem_free(tmp->val);
    hnode_free(tmp);
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
  if (htable->capacity == htable->size && !rehash(htable, htable->size * 2)) {
    return;
  }

  size_t idx = hash(htable, key);
  struct helem *entry = *lookup(htable, key, idx);
  if (entry != NULL) { /* The key already exists in the hash table */
    memcpy(entry->val, val, htable->vsize);
    return;
  }
  entry = malloc(sizeof(*entry));
  entry->key = malloc(htable->ksize);
  entry->val = malloc(htable->vsize);
  struct hnode *hnode = hnode_create(NULL, entry);

  if (!entry || !entry->key || !entry->val || !hnode) {
    YU_LOG_ERROR("Failed to allocate memory for hash entry\n");
    free(entry);
    free(hnode);
    free(entry->key);
    free(entry->val);
    return;
  }
  *(htable->tail) = hnode;
  entry->node = htable->tail;
  htable->tail = &(*htable->tail)->next;

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
  if (*entry == NULL) {
    return;
  }
  struct hnode **cur_node = (*entry)->node;
  struct helem *free_elem = *entry;
  struct hnode *free_node = *cur_node;

  *cur_node = (*cur_node)->next;
  if (*cur_node != NULL) {
    (*cur_node)->val->node = cur_node;
  } else {
    htable->tail = cur_node;
  }
  *entry = (*entry)->next;

  hnode_free(free_node);
  helem_free(free_elem);
  htable->size--;
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
