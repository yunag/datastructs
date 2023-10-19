#include "datastructs/hash_table.h"
#include "datastructs/memory.h"
#include "datastructs/types.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Should be arranged from 0.5 to 0.8 */
#define IDEAL_LOAD_FACTOR 0.7

#define GET_BUCKET_BY_HASHV(htable, key)                                       \
  (&htable->buckets[(key)->hashv % (htable)->capacity])

#define GET_BUCKET(htable, key)                                                \
  (&htable->buckets[((key)->hashv = htable->hash(key)) % (htable)->capacity])

/* Pointer to invalid memory address */
static unsigned char ___dummy_ptr;
#define DUMMY_PTR ((void *)&___dummy_ptr)

struct hash_table {
  struct hash_bucket *buckets; /* Buckets to store pointers to hash entrys */
  struct hash_entry head;      /* Head of `global` linked list */

  hash_entry_fun hash;
  compare_ht_fun cmp;
  destroy_ht_fun destroy;

  size_t ideal_size; /* Size of the hash table should not be greater than this
                        value */

  size_t size;     /* Number of items in the table */
  size_t capacity; /* Capacity of the table */
};

bool htable_rehash(hash_table *htable, size_t newsize) {
  assert(htable != NULL);

  struct hash_bucket *nbuckets =
      _yu_allocator.calloc(newsize, sizeof(*nbuckets));
  if (!nbuckets) {
    return false;
  }

  _yu_allocator.free(htable->buckets);

  htable->buckets = nbuckets;
  htable->capacity = newsize;
  htable->ideal_size = newsize * IDEAL_LOAD_FACTOR + 1;

  struct hash_entry *entry = htable->head.ht_next;
  while (entry != &htable->head) {
    struct hash_bucket *bucket = GET_BUCKET_BY_HASHV(htable, entry);

    entry->next = bucket->entry;
    bucket->entry = entry;

    entry = entry->ht_next;
  }

  return true;
}

static struct hash_entry **
htable_lookup_internal(hash_table *htable, const struct hash_entry *query,
                       struct hash_bucket *bucket) {
  struct hash_entry **entry = &bucket->entry;
  while (*entry &&
         ((*entry)->hashv != query->hashv || htable->cmp(*entry, query))) {
    entry = &(*entry)->next;
  }
  return entry;
}

static void htable_replace_entry(hash_table *htable, struct hash_entry **victim,
                                 struct hash_entry *new) {
  struct hash_entry *destroy_entry = *victim;
  *new = *destroy_entry;
  *victim = new;
  if (htable->destroy) {
    htable->destroy(destroy_entry);
  }
  new->ht_prev->ht_next = new;
  new->ht_next->ht_prev = new;
}

hash_table *htable_create(size_t capacity, hash_entry_fun hash,
                          compare_ht_fun cmp, destroy_ht_fun destroy) {
  assert(capacity > 0);
  assert(hash != NULL);
  assert(cmp != NULL);

  hash_table *htable = _yu_allocator.allocate(sizeof(*htable));
  if (!htable) {
    return NULL;
  }

  htable->buckets = _yu_allocator.calloc(capacity, sizeof(*htable->buckets));
  if (!htable->buckets) {
    _yu_allocator.free(htable);
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
    struct hash_entry *entry = htable->head.ht_next;
    while (entry != &htable->head) {
      struct hash_entry *tmp = entry;
      entry = entry->ht_next;
      htable->destroy(tmp);
    }
  }
  _yu_allocator.free(htable->buckets);
  _yu_allocator.free(htable);
}

bool htable_insert(hash_table *htable, struct hash_entry *hentry) {
  assert(htable != NULL);
  assert(hentry != NULL);

  if (htable->size == htable->ideal_size &&
      !htable_rehash(htable, htable->capacity * 2)) {
    return false;
  }

  struct hash_bucket *bucket = GET_BUCKET(htable, hentry);
  struct hash_entry **link = htable_lookup_internal(htable, hentry, bucket);

  if (*link) { /* The key already exists in the hash table */
    htable_replace_entry(htable, link, hentry);
    return true;
  }

  struct hash_entry *tail = htable->head.ht_prev;
  hentry->ht_prev = tail;
  hentry->ht_next = tail->ht_next;
  tail->ht_next->ht_prev = hentry;
  tail->ht_next = hentry;

  hentry->next = bucket->entry;
  bucket->entry = hentry;
  htable->size++;
  return true;
}

struct hash_entry *htable_lookup(hash_table *htable, struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  return *htable_lookup_internal(htable, query, GET_BUCKET(htable, query));
}

static bool htable_remove_link(hash_table *htable, struct hash_entry **link) {
  struct hash_entry *entry = *link;

  if (!entry) {
    return false;
  }
  entry->ht_prev->ht_next = entry->ht_next;
  entry->ht_next->ht_prev = entry->ht_prev;
  *link = (*link)->next;

  if (htable->destroy) {
    htable->destroy(entry);
  }
  htable->size--;
  return true;
}

bool htable_remove(hash_table *htable, struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  struct hash_entry **link =
      htable_lookup_internal(htable, query, GET_BUCKET(htable, query));

  return htable_remove_link(htable, link);
}

bool htable_delete(hash_table *htable, struct hash_entry *hentry) {
  assert(htable != NULL);
  assert(hentry != NULL);

  struct hash_bucket *bucket = GET_BUCKET_BY_HASHV(htable, hentry);
  struct hash_entry **link = &bucket->entry;

  while (*link && *link != hentry) {
    link = &(*link)->ht_next;
  }
  return htable_remove_link(htable, link);
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

/* Simon Tatham's algorithm */
void htable_sort(hash_table *htable, compare_ht_fun cmp) {
  assert(htable != NULL);
  assert(cmp != NULL);

  struct hash_entry *p, *q, *e;
  struct hash_entry *head = htable->head.ht_next;
  struct hash_entry *tail = htable->head.ht_prev;

  size_t insize;
  size_t psize, qsize;
  size_t i;

  if (!htable->size)
    return;

  tail->ht_next = head->ht_prev = NULL;

  insize = 1;

  while (insize < htable->size) {
    p = head;
    head = tail = NULL;

    while (p) {
      q = p;

      for (i = 0; i < insize && q; i++) {
        q = q->ht_next;
      }
      psize = i;
      qsize = insize;

      while (psize > 0 || (qsize > 0 && q)) {
        if (psize == 0) {
          e = q;
          q = q->ht_next;
          qsize--;
        } else if (qsize == 0 || !q) {
          e = p;
          p = p->ht_next;
          psize--;
        } else if (cmp(p, q) < 0) {
          e = p;
          p = p->ht_next;
          psize--;
        } else {
          e = q;
          q = q->ht_next;
          qsize--;
        }

        if (tail) {
          tail->ht_next = e;
        } else {
          head = e;
        }
        e->ht_prev = tail;
        tail = e;
      }

      p = q;
    }
    tail->ht_next = NULL;
    insize *= 2;
  }

  tail->ht_next = head->ht_prev = &htable->head;
  htable->head.ht_next = head;
  htable->head.ht_prev = tail;
}
