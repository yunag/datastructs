#include "datastructs/hash_table.h"
#include "datastructs/memory.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Should be arranged from 0.5 to 0.8 */
#define IDEAL_LOAD_FACTOR 0.7

#define HT_BUCKET_BY_HASHV(htable, key)                                        \
  (&(htable)->buckets[(key)->hashv % (htable)->num_buckets])

#define HT_BUCKET(htable, key)                                                 \
  (&(htable)->buckets[((key)->hashv = (htable)->hash(key)) %                   \
                      (htable)->num_buckets])

#define HT_HEAD(htable) ((htable)->head.ht_next)
#define HT_TAIL(htable) ((htable)->head.ht_prev)

#define HT_SET_HEAD(htable, entry) (HT_HEAD(htable) = (entry))
#define HT_SET_TAIL(htable, entry) (HT_TAIL(htable) = (entry))

/* Pointer to invalid memory address */
static unsigned char ___dummy_ptr;
#define DUMMY_PTR ((void *)&___dummy_ptr)

struct hash_table {
  struct hash_bucket *buckets; /* Buckets to store pointers to hash entrys */
  struct hash_entry head;      /* Dummy head of `global` linked list */

  hash_entry_fun hash;
  lookup_ht_fun lookup;

  size_t ideal_size; /* Size of the hash table should not be greater than this
                        value */

  size_t size;        /* Number of items in the table */
  size_t num_buckets; /* Capacity of the table */
};

bool htable_rehash(hash_table *htable, size_t newsize) {
  assert(htable != NULL);

  struct hash_bucket *nbuckets = yu_calloc(newsize, sizeof(*nbuckets));
  if (!nbuckets) {
    return false;
  }

  yu_free(htable->buckets);

  htable->buckets = nbuckets;
  htable->num_buckets = newsize;
  htable->ideal_size = newsize * IDEAL_LOAD_FACTOR + 1;

  struct hash_entry *entry = HT_HEAD(htable);
  while (entry != &htable->head) {
    struct hash_bucket *bucket = HT_BUCKET_BY_HASHV(htable, entry);

    entry->next = bucket->entry;
    bucket->entry = entry;

    entry = entry->ht_next;
  }

  return true;
}

static void htable_replace_entry(struct hash_entry **victim,
                                 struct hash_entry *new) {
  struct hash_entry *entry = *victim;

  new->ht_prev = entry->ht_prev;
  new->ht_next = entry->ht_next;
  new->next = entry->next;

  *victim = new;

  new->ht_prev->ht_next = new;
  new->ht_next->ht_prev = new;
}

hash_table *htable_create(size_t capacity, hash_entry_fun hash,
                          lookup_ht_fun lookup) {
  assert(capacity > 0);
  assert(hash != NULL);
  assert(lookup != NULL);

  hash_table *htable = yu_malloc(sizeof(*htable));
  if (!htable) {
    return NULL;
  }

  htable->buckets = yu_calloc(capacity, sizeof(*htable->buckets));
  if (!htable->buckets) {
    yu_free(htable);
    return NULL;
  }

  htable->lookup = lookup;
  htable->hash = hash;
  htable->head.ht_next = htable->head.ht_prev = &htable->head;

  htable->head.next = DUMMY_PTR;

  htable->size = 0;
  htable->ideal_size = capacity * IDEAL_LOAD_FACTOR + 1;
  htable->num_buckets = capacity;
  return htable;
}

void htable_destroy(hash_table *htable, destroy_ht_fun destroy_entry) {
  if (!htable) {
    return;
  }

  if (destroy_entry) {
    struct hash_entry *entry = htable->head.ht_next;
    while (entry != &htable->head) {
      struct hash_entry *tmp = entry;
      entry = entry->ht_next;
      destroy_entry(tmp);
    }
  }
  yu_free(htable->buckets);
  yu_free(htable);
}

static inline bool htable_expand_buckets(hash_table *htable) {
  return htable->size >= htable->ideal_size &&
         !htable_rehash(htable, htable->num_buckets * 2);
}

static void htable_link_entry(struct hash_entry *tail,
                              struct hash_entry *hentry,
                              struct hash_bucket *bucket) {
  hentry->ht_prev = tail;
  hentry->ht_next = tail->ht_next;
  tail->ht_next->ht_prev = hentry;
  tail->ht_next = hentry;

  hentry->next = bucket->entry;
  bucket->entry = hentry;
}

bool htable_insert(hash_table *htable, struct hash_entry *hentry) {
  assert(htable != NULL);
  assert(hentry != NULL);

  if (htable_expand_buckets(htable)) {
    return false;
  }

  struct hash_bucket *bucket = HT_BUCKET(htable, hentry);
  struct hash_entry *tail = HT_TAIL(htable);

  htable_link_entry(tail, hentry, bucket);
  htable->size++;

  return true;
}

bool htable_replace(hash_table *htable, struct hash_entry *hentry,
                    struct hash_entry **replaced) {
  assert(htable != NULL);
  assert(hentry != NULL);
  assert(replaced != NULL);

  *replaced = NULL;

  if (htable_expand_buckets(htable)) {
    return false;
  }

  struct hash_bucket *bucket = HT_BUCKET(htable, hentry);
  struct hash_entry *tail = HT_TAIL(htable);
  struct hash_entry **link = htable->lookup(hentry, bucket);

  if (*link) {
    *replaced = *link;

    htable_replace_entry(link, hentry);
    return true;
  }

  htable_link_entry(tail, hentry, bucket);
  htable->size++;

  return true;
}

struct hash_entry *htable_lookup(hash_table *htable, struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  struct hash_bucket *bucket = HT_BUCKET(htable, query);

  return *htable->lookup(query, bucket);
}

static void htable_remove_link(struct hash_entry **link) {
  struct hash_entry *entry = *link;

  assert(entry != NULL);

  entry->ht_prev->ht_next = entry->ht_next;
  entry->ht_next->ht_prev = entry->ht_prev;
  *link = (*link)->next;
}

struct hash_entry *htable_remove(hash_table *htable, struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  struct hash_bucket *bucket = HT_BUCKET(htable, query);
  struct hash_entry **link = htable->lookup(query, bucket);
  struct hash_entry *entry = *link;

  if (*link) {
    htable_remove_link(link);
    htable->size--;
  }

  return entry;
}

bool htable_delete(hash_table *htable, struct hash_entry *hentry) {
  assert(htable != NULL);
  assert(hentry != NULL);

  struct hash_bucket *bucket = HT_BUCKET_BY_HASHV(htable, hentry);
  struct hash_entry **link = &bucket->entry;

  while (*link && *link != hentry) {
    link = &(*link)->ht_next;
  }

  if (*link) {
    htable_remove_link(link);
    htable->size--;
    return true;
  }

  return false;
}

size_t htable_size(hash_table *htable) {
  assert(htable != NULL);
  return htable->size;
}

struct hash_entry *htable_first(hash_table *htable) {
  assert(htable != NULL);
  return HT_HEAD(htable);
}

struct hash_entry *htable_last(hash_table *htable) {
  assert(htable != NULL);
  return HT_TAIL(htable);
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
  struct hash_entry *head = HT_HEAD(htable);
  struct hash_entry *tail = HT_TAIL(htable);

  size_t insize = 1;
  size_t psize, qsize;

  tail->ht_next = head->ht_prev = NULL;

  while (insize < htable->size) {
    p = head;
    head = tail = NULL;

    while (p) {
      q = p;

      for (psize = 0; psize < insize && q; psize++) {
        q = q->ht_next;
      }
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
  HT_SET_HEAD(htable, head);
  HT_SET_TAIL(htable, tail);
}
