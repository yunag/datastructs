#include "datastructs/hash_table.h"
#include "datastructs/utils.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define GET_BUCKET(htable, key)                                                \
  ((htable)->hash((key), (htable)->ksize) % (htable)->capacity)

struct hash_entry {
  void *key;               /* Pointer to key copy */
  void *val;               /* Pointer to value copy */
  struct hash_entry *next; /* Pointer to next struct */
  struct hash_node **node;
};

struct hash_node {
  struct hash_node *next;
  struct hash_entry *entry;
};

struct hash_table {
  struct hash_entry **buckets; /* Buckets to store pointers to hash entrys */
  struct hash_node head;       /* Head of `global` linked list */
  struct hash_node **tail;     /* Tail of `global` linked list */
  hash_fn hash;                /* Hash function */
  free_fn vfree;               /* Function to free a value */
  size_t ksize;                /* Size of a key */
  size_t vsize;                /* Size of a value */
  size_t size;                 /* Number of elements*/
  size_t capacity;             /* Capacity of the table */
};

static uint64_t hash(const void *key, size_t size) {
  const unsigned char *bytes = key;
  uint64_t hash = 5381;
  for (size_t i = 0; i < size; ++i) {
    hash = (hash << 5) + hash + bytes[i];
  }
  return hash;
}

static inline void free_placeholder(void *const *value) { YU_UNUSED(value); }

static void hash_entry_free(struct hash_entry *hentry) {
  free(hentry->key);
  free(hentry->val);
  free(hentry);
}

static struct hash_node *hnode_create(struct hash_entry *val) {
  struct hash_node *node = malloc(sizeof(*node));
  if (node == NULL) {
    YU_LOG_ERROR("Failed to create hash node\n");
    return NULL;
  }
  node->next = NULL;
  node->entry = val;
  return node;
}

static void hnode_free(struct hash_node *node) { free(node); }

static bool rehash(hash_table *htable, size_t newsize) {
  struct hash_entry **nbuckets = calloc(newsize, sizeof(struct hash_entry *));
  if (nbuckets == NULL) {
    YU_LOG_ERROR("Failed to resize the hash table to %zu\n", newsize);
    return false;
  }
  free(htable->buckets);
  htable->buckets = nbuckets;
  htable->capacity = newsize;

  struct hash_node *head = htable->head.next;
  while (head != NULL) {
    struct hash_entry *entry = head->entry;
    uint64_t bct = GET_BUCKET(htable, entry->key);

    entry->next = htable->buckets[bct];
    htable->buckets[bct] = entry;

    head = head->next;
  }

  return true;
}

hash_table *htable_create(size_t table_size, size_t key_size, size_t value_size,
                          hash_fn user_hash, free_fn vfree) {
  assert(table_size > 0);
  assert(key_size > 0);
  assert(value_size > 0);

  hash_table *htable = malloc(sizeof(*htable));
  if (htable == NULL) {
    YU_LOG_ERROR("Failed to allocate memory for hash table\n");
    return NULL;
  }
  htable->buckets = calloc(table_size, sizeof(*htable->buckets));
  if (htable->buckets == NULL) {
    free(htable);
    YU_LOG_ERROR("Failed to allocate memory for table\n");
    return NULL;
  }
  htable->hash = user_hash ? user_hash : hash;
  htable->vfree = vfree ? vfree : free_placeholder;
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
  struct hash_node *head = htable->head.next;
  while (head != NULL) {
    struct hash_node *tmp = head;
    head = head->next;

    htable->vfree(tmp->entry->val);
    hash_entry_free(tmp->entry);
    hnode_free(tmp);
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
  struct hash_node *hnode = hnode_create(entry);

  if (!entry || !entry->key || !entry->val || !hnode) {
    hash_entry_free(entry);
    hnode_free(hnode);
    YU_LOG_ERROR("Failed to allocate memory for hash entry\n");
    return;
  }
  *(htable->tail) = hnode;
  entry->node = htable->tail;
  htable->tail = &(*htable->tail)->next;

  memcpy(entry->key, key, htable->ksize);
  memcpy(entry->val, val, htable->vsize);

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

  struct hash_entry **entry = lookup(htable, key, GET_BUCKET(htable, key));
  if (*entry == NULL) {
    return;
  }
  struct hash_node **cur_node = (*entry)->node;
  struct hash_entry *free_elem = *entry;
  struct hash_node *free_node = *cur_node;

  *cur_node = (*cur_node)->next;
  if (*cur_node != NULL) {
    (*cur_node)->entry->node = cur_node;
  } else {
    htable->tail = cur_node;
  }
  *entry = (*entry)->next;

  htable->vfree(free_elem->val);
  hash_entry_free(free_elem);
  hnode_free(free_node);
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
