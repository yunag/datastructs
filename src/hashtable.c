#include "datastructs/hash_table.h"
#include "datastructs/memory.h"
#include "datastructs/types.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

#define IDEAL_LOAD_FACTOR 0.7

#define GET_BUCKET(htable, key) ((htable)->hash((key)) % (htable)->capacity)

/* Pointer to invalid memory address */
static unsigned char ___dummy_ptr;
#define DUMMY_PTR ((void *)&___dummy_ptr)

struct hash_table {
  struct hash_entry **buckets; /* Buckets to store pointers to hash entrys */
  struct hash_entry head;      /* Head of `global` linked list */

  hash_entry_fun hash;
  compare_ht_fun cmp;
  destroy_ht_fun destroy;

  size_t ideal_size; /* Size of the hash table should not be greater than this
                        value */

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
  htable->ideal_size = newsize * IDEAL_LOAD_FACTOR + 1;

  struct hash_entry *walk = htable->head.ht_next;
  while (walk != &htable->head) {
    uint64_t bct = GET_BUCKET(htable, walk);

    walk->next = htable->buckets[bct];
    htable->buckets[bct] = walk;

    walk = walk->ht_next;
  }

  return true;
}

static struct hash_entry **
htable_lookup_internal(hash_table *htable, const struct hash_entry *query,
                       uint64_t bucket) {
  struct hash_entry **walk = &htable->buckets[bucket];
  while (*walk && htable->cmp(*walk, query)) {
    walk = &(*walk)->next;
  }
  return walk;
}

static void htable_replace_entry(hash_table *htable, struct hash_entry **victim,
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

hash_table *htable_create(size_t capacity, hash_entry_fun hash,
                          compare_ht_fun cmp, destroy_ht_fun destroy) {
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
  htable->ideal_size = capacity * IDEAL_LOAD_FACTOR + 1;
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

bool htable_insert(hash_table *htable, struct hash_entry *hentry) {
  assert(htable != NULL);
  assert(hentry != NULL);

  if (htable->size == htable->ideal_size &&
      !rehash(htable, htable->capacity * 2)) {
    return false;
  }

  uint64_t bct = GET_BUCKET(htable, hentry);
  struct hash_entry **link = htable_lookup_internal(htable, hentry, bct);
  if (*link) { /* The key already exists in the hash table */
    htable_replace_entry(htable, link, hentry);
    return true;
  }
  struct hash_entry *tail = htable->head.ht_prev;
  hentry->ht_prev = tail;
  hentry->ht_next = tail->ht_next;
  tail->ht_next->ht_prev = hentry;
  tail->ht_next = hentry;

  hentry->next = htable->buckets[bct];
  htable->buckets[bct] = hentry;
  htable->size++;
  return true;
}

struct hash_entry *htable_lookup(hash_table *htable,
                                 const struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  struct hash_entry *entry =
      *htable_lookup_internal(htable, query, GET_BUCKET(htable, query));
  return entry;
}

bool htable_remove(hash_table *htable, const struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  struct hash_entry **link =
      htable_lookup_internal(htable, query, GET_BUCKET(htable, query));
  if (!*link) {
    return false;
  }
  struct hash_entry *entry = *link;
  entry->ht_prev->ht_next = entry->ht_next;
  entry->ht_next->ht_prev = entry->ht_prev;
  *link = (*link)->next;

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

struct hash_entry *htable_first(hash_table *htable) {
  assert(htable != NULL);
  return htable->head.ht_next;
}

struct hash_entry *htable_last(hash_table *htable) {
  assert(htable != NULL);
  return htable->head.ht_prev;
}

struct hash_entry *htable_next(const struct hash_entry *entry) {
  assert(entry != NULL);

  entry = entry->ht_next;
  if (entry->next == DUMMY_PTR) {
    entry = NULL;
  }
  return (struct hash_entry *)entry;
}

struct hash_entry *htable_prev(const struct hash_entry *entry) {
  assert(entry != NULL);

  entry = entry->ht_prev;
  if (entry->next == DUMMY_PTR) {
    entry = NULL;
  }
  return (struct hash_entry *)entry;
}
