#include "datastructs/hash_table.h"
#include "datastructs/memory.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Should be arranged from 0.5 to 0.8 */
#define IDEAL_LOAD_FACTOR 0.7

#define HT_HEAD(htable) ((htable)->dummy_head.ht_next)
#define HT_TAIL(htable) ((htable)->dummy_head.ht_prev)

#define HT_SET_HEAD(htable, entry) (HT_HEAD(htable) = (entry))
#define HT_SET_TAIL(htable, entry) (HT_TAIL(htable) = (entry))

/* Pointer to an invalid memory address used for determining dummy head */
static unsigned char dummy_ptr__;
#define DUMMY_PTR ((void *)&dummy_ptr__)

struct hash_table {
  struct hash_bucket *buckets;  /* Buckets to store pointers to hash entrys */
  struct hash_entry dummy_head; /* Dummy head of `global` linked list */

  ht_hash_fun hash;
  ht_equal_fun equal;

  /* Number of items in the hash table should not be
   * greater than this value */
  size_t ideal_num_items;

  size_t num_items;   /* Number of items in the table */
  size_t num_buckets; /* Number of buckets in the table */
};

static inline struct hash_bucket *htable_bucket_by_hashv(hash_table *htable,
                                                         size_t hashv) {
  return &htable->buckets[hashv % htable->num_buckets];
}

static inline struct hash_bucket *htable_bucket(hash_table *htable,
                                                struct hash_entry *entry) {
  entry->hashv = htable->hash(entry);

  return htable_bucket_by_hashv(htable, entry->hashv);
}

static inline bool htable_expand_buckets(hash_table *htable) {
  return htable->num_items < htable->ideal_num_items ||
         htable_rehash(htable, htable->num_buckets * 2);
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

static void htable_link_entry(struct hash_entry *tail, struct hash_entry *entry,
                              struct hash_bucket *bucket) {
  entry->ht_prev = tail;
  entry->ht_next = tail->ht_next;
  tail->ht_next->ht_prev = entry;
  tail->ht_next = entry;

  entry->next = bucket->entry;
  bucket->entry = entry;
}

static struct hash_entry **htable_lookup_in_bucket(hash_table *htable,
                                                   struct hash_bucket *bucket,
                                                   struct hash_entry *query) {
  struct hash_entry **link = &bucket->entry;

  while (*link) {
    struct hash_entry *entry = *link;
    if (entry->hashv == query->hashv && htable->equal(entry, query)) {
      break;
    }

    link = &entry->next;
  }

  return link;
}

hash_table *htable_create(size_t num_buckets, ht_hash_fun hash,
                          ht_equal_fun equal) {
  assert(num_buckets > 0);
  assert(hash != NULL);
  assert(equal != NULL);

  hash_table *htable = yu_malloc(sizeof(*htable));
  if (!htable) {
    return NULL;
  }

  htable->buckets = yu_calloc(num_buckets, sizeof(*htable->buckets));
  if (!htable->buckets) {
    yu_free(htable);
    return NULL;
  }

  htable->equal = equal;
  htable->hash = hash;
  htable->dummy_head.ht_next = htable->dummy_head.ht_prev = &htable->dummy_head;

  htable->dummy_head.next = DUMMY_PTR;

  htable->num_items = 0;
  htable->ideal_num_items = num_buckets * IDEAL_LOAD_FACTOR + 1;
  htable->num_buckets = num_buckets;

  return htable;
}

void htable_destroy(hash_table *htable, ht_destroy_fun destroy_table) {
  if (!htable) {
    return;
  }

  if (destroy_table) {
    destroy_table(htable);
  }

  yu_free(htable->buckets);
  yu_free(htable);
}

bool htable_rehash(hash_table *htable, size_t newsize) {
  assert(htable != NULL);

  struct hash_bucket *nbuckets = yu_calloc(newsize, sizeof(*nbuckets));
  if (!nbuckets) {
    return false;
  }

  yu_free(htable->buckets);

  htable->buckets = nbuckets;
  htable->num_buckets = newsize;
  htable->ideal_num_items = newsize * IDEAL_LOAD_FACTOR + 1;

  struct hash_entry *entry = HT_HEAD(htable);
  while (entry != &htable->dummy_head) {
    struct hash_bucket *bucket = htable_bucket_by_hashv(htable, entry->hashv);

    entry->next = bucket->entry;
    bucket->entry = entry;

    entry = entry->ht_next;
  }

  return true;
}

bool htable_insert(hash_table *htable, struct hash_entry *entry) {
  assert(htable != NULL);
  assert(entry != NULL);

  if (!htable_expand_buckets(htable)) {
    return false;
  }

  struct hash_bucket *bucket = htable_bucket(htable, entry);
  struct hash_entry *tail = HT_TAIL(htable);

  htable_link_entry(tail, entry, bucket);
  htable->num_items++;

  return true;
}

bool htable_replace(hash_table *htable, struct hash_entry *entry,
                    struct hash_entry **replaced) {
  assert(htable != NULL);
  assert(entry != NULL);
  assert(replaced != NULL);

  *replaced = NULL;

  if (!htable_expand_buckets(htable)) {
    return false;
  }

  struct hash_bucket *bucket = htable_bucket(htable, entry);
  struct hash_entry **link = htable_lookup_in_bucket(htable, bucket, entry);
  struct hash_entry *tail = HT_TAIL(htable);

  if (*link) {
    *replaced = *link;

    htable_replace_entry(link, entry);
    return true;
  }

  htable_link_entry(tail, entry, bucket);
  htable->num_items++;

  return true;
}

struct hash_entry *htable_lookup(hash_table *htable, struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  struct hash_bucket *bucket = htable_bucket(htable, query);

  return *htable_lookup_in_bucket(htable, bucket, query);
}

static void htable_remove_link(struct hash_entry **link) {
  struct hash_entry *entry = *link;

  entry->ht_prev->ht_next = entry->ht_next;
  entry->ht_next->ht_prev = entry->ht_prev;
  *link = (*link)->next;
}

struct hash_entry *htable_remove(hash_table *htable, struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  struct hash_bucket *bucket = htable_bucket(htable, query);
  struct hash_entry **link = htable_lookup_in_bucket(htable, bucket, query);

  struct hash_entry *entry = *link;

  if (entry) {
    htable_remove_link(link);
    htable->num_items--;
  }

  return entry;
}

void htable_erase(hash_table *htable, struct hash_entry *entry) {
  assert(htable != NULL);
  assert(entry != NULL);

  struct hash_bucket *bucket = htable_bucket_by_hashv(htable, entry->hashv);
  struct hash_entry **link = &bucket->entry;

  while (*link != entry) {
    link = &(*link)->next;
  }

  htable_remove_link(link);
  htable->num_items--;
}

size_t htable_size(hash_table *htable) {
  assert(htable != NULL);
  return htable->num_items;
}

struct hash_entry *htable_first(hash_table *htable) {
  assert(htable != NULL);
  return htable->num_items ? HT_HEAD(htable) : NULL;
}

struct hash_entry *htable_last(hash_table *htable) {
  assert(htable != NULL);
  return htable->num_items ? HT_TAIL(htable) : NULL;
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
void htable_sort(hash_table *htable, ht_less_fun less) {
  assert(htable != NULL);
  assert(less != NULL);

  struct hash_entry *p, *q, *e;
  struct hash_entry *head = HT_HEAD(htable);
  struct hash_entry *tail = HT_TAIL(htable);

  size_t insize = 1;
  size_t psize, qsize;

  tail->ht_next = head->ht_prev = NULL;

  while (insize < htable->num_items) {
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
        } else if (less(p, q)) {
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

  tail->ht_next = head->ht_prev = &htable->dummy_head;
  HT_SET_HEAD(htable, head);
  HT_SET_TAIL(htable, tail);
}
