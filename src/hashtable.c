#include "datastructs/hash_table.h"
#include "datastructs/memory.h"
#include "datastructs/types.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

#define GET_BUCKET(htable, key) ((htable)->hash((key)) % (htable)->capacity)

/* Pointer to invalid memory address */
static unsigned char ___dummy_ptr;
#define DUMMY_PTR ((void *)&___dummy_ptr)

struct hash_table {
  struct hash_entry **buckets; /* Buckets to store pointers to hash entrys */
  struct hash_entry head;      /* Head of `global` linked list */

  hash_ht_entries_fn hash;     /* Hash entry */
  cmp_ht_entries_fn cmp;       /* Compare two entries */
  destroy_ht_entry_fn destroy; /* Destory entry */

  size_t size;     /* Number of elements*/
  size_t capacity; /* Capacity of the table */
};

static bool rehash(hash_table *htable, size_t newsize) {
  struct hash_entry **nbuckets = yu_calloc(newsize, sizeof(*nbuckets));
  if (!nbuckets) {
    return false;
  }
  yu_free(htable->buckets);
  htable->buckets = nbuckets;
  htable->capacity = newsize;

  struct hash_entry *walk = htable->head.ht_next;
  while (walk != &htable->head) {
    uint64_t bct = GET_BUCKET(htable, walk);

    walk->next = htable->buckets[bct];
    htable->buckets[bct] = walk;

    walk = walk->ht_next;
  }

  return true;
}

hash_table *htable_create(size_t capacity, hash_ht_entries_fn hash,
                          cmp_ht_entries_fn cmp, destroy_ht_entry_fn destroy) {
  assert(capacity > 0);
  assert(hash != NULL);
  assert(cmp != NULL);

  hash_table *htable = yu_allocate(sizeof(*htable));
  if (!htable) {
    return NULL;
  }
  htable->buckets = yu_calloc(capacity, sizeof(*htable->buckets));
  if (!htable->buckets) {
    yu_free(htable);
    return NULL;
  }
  htable->cmp = cmp;
  htable->hash = hash;
  htable->destroy = destroy;
  htable->head.ht_next = htable->head.ht_prev = &htable->head;
  htable->head.next = DUMMY_PTR;

  htable->size = 0;
  htable->capacity = capacity;
  return htable;
}

void htable_destroy(hash_table *htable) {
  if (!htable) {
    return;
  }

  if (htable->destroy) {
    struct hash_entry *walk = htable->head.ht_next;
    while (walk != &htable->head) {
      struct hash_entry *tmp = walk;
      walk = walk->ht_next;
      htable->destroy(tmp);
    }
  }
  yu_free(htable->buckets);
  yu_free(htable);
}

static struct hash_entry **
lookup(hash_table *htable, const struct hash_entry *query, uint64_t bucket) {
  struct hash_entry **walk = &htable->buckets[bucket];
  while (*walk && htable->cmp(*walk, query)) {
    walk = &(*walk)->next;
  }
  return walk;
}

void replace_entry(hash_table *htable, struct hash_entry **victim,
                   struct hash_entry *new) {
  struct hash_entry *destroy_node = *victim;
  *new = *destroy_node;
  *victim = new;
  if (htable->destroy) {
    htable->destroy(destroy_node);
  }
  new->ht_prev->ht_next = new;
  new->ht_next->ht_prev = new;
}

bool htable_insert(hash_table *htable, struct hash_entry *ht_entry) {
  assert(htable != NULL);
  assert(ht_entry != NULL);

  if (htable->capacity == htable->size && !rehash(htable, htable->size * 2)) {
    return false;
  }
  uint64_t bct = GET_BUCKET(htable, ht_entry);
  struct hash_entry **entry = lookup(htable, ht_entry, bct);
  if (*entry) { /* The key already exists in the hash table */
    replace_entry(htable, entry, ht_entry);
    return true;
  }
  struct hash_entry *tail = htable->head.ht_prev;
  ht_entry->ht_prev = tail;
  ht_entry->ht_next = tail->ht_next;
  tail->ht_next->ht_prev = ht_entry;
  tail->ht_next = ht_entry;

  ht_entry->next = htable->buckets[bct];
  htable->buckets[bct] = ht_entry;
  htable->size++;
  return true;
}

struct hash_entry *htable_lookup(hash_table *htable,
                                 const struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  struct hash_entry *entry = *lookup(htable, query, GET_BUCKET(htable, query));
  return entry;
}

bool htable_remove(hash_table *htable, const struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  struct hash_entry **fentry = lookup(htable, query, GET_BUCKET(htable, query));
  if (!*fentry) {
    return false;
  }
  struct hash_entry *entry = *fentry;
  entry->ht_prev->ht_next = entry->ht_next;
  entry->ht_next->ht_prev = entry->ht_prev;
  *fentry = (*fentry)->next;

  if (htable->destroy) {
    htable->destroy(entry);
  }
  htable->size--;
  return true;
}

size_t htable_size(hash_table *htable) {
  assert(htable != NULL);
  return htable->size;
}

struct hash_entry *ht_first(hash_table *htable) {
  assert(htable != NULL);
  return htable->head.ht_next;
}

struct hash_entry *ht_last(hash_table *htable) {
  assert(htable != NULL);
  return htable->head.ht_prev;
}

struct hash_entry *ht_next(struct hash_entry *entry) {
  assert(entry != NULL);

  entry = entry->ht_next;
  if (entry->next == DUMMY_PTR) {
    entry = NULL;
  }
  return entry;
}

struct hash_entry *ht_prev(struct hash_entry *entry) {
  assert(entry != NULL);

  entry = entry->ht_prev;
  if (entry->next == DUMMY_PTR) {
    entry = NULL;
  }
  return entry;
}
