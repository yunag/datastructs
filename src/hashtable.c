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

/* Pointer to an invalid memory address used for determining dummy head */
static unsigned char ___dummy_ptr;
#define DUMMY_PTR ((void *)&___dummy_ptr)

struct hash_table {
  struct hash_bucket *buckets; /* Buckets to store pointers to hash entrys */
  struct hash_entry head;      /* Dummy head of `global` linked list */

  hash_entry_fun hash;
  equal_ht_fun equal;

  /* Number of items in the hash table should not be
   * greater than this value */
  size_t ideal_num_items;

  size_t num_items;   /* Number of items in the table */
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
  htable->ideal_num_items = newsize * IDEAL_LOAD_FACTOR + 1;

  struct hash_entry *entry = HT_HEAD(htable);
  while (entry != &htable->head) {
    struct hash_bucket *bucket = HT_BUCKET_BY_HASHV(htable, entry);

    entry->next = bucket->entry;
    bucket->entry = entry;

    entry = entry->ht_next;
  }

  return true;
}

hash_table *htable_create(size_t capacity, hash_entry_fun hash,
                          equal_ht_fun equal) {
  assert(capacity > 0);
  assert(hash != NULL);
  assert(equal != NULL);

  hash_table *htable = yu_malloc(sizeof(*htable));
  if (!htable) {
    return NULL;
  }

  htable->buckets = yu_calloc(capacity, sizeof(*htable->buckets));
  if (!htable->buckets) {
    yu_free(htable);
    return NULL;
  }

  htable->equal = equal;
  htable->hash = hash;
  htable->head.ht_next = htable->head.ht_prev = &htable->head;

  htable->head.next = DUMMY_PTR;

  htable->num_items = 0;
  htable->ideal_num_items = capacity * IDEAL_LOAD_FACTOR + 1;
  htable->num_buckets = capacity;

  return htable;
}

void htable_destroy(hash_table *htable, destroy_table_fun destroy_table) {
  if (!htable) {
    return;
  }

  if (destroy_table) {
    destroy_table(htable);
  }

  yu_free(htable->buckets);
  yu_free(htable);
}

static inline bool htable_expand_buckets(hash_table *htable) {
  return htable->num_items >= htable->ideal_num_items &&
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

static struct hash_entry **htable_lookup_bucket(hash_table *htable,
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

bool htable_insert(hash_table *htable, struct hash_entry *hentry) {
  assert(htable != NULL);
  assert(hentry != NULL);

  if (htable_expand_buckets(htable)) {
    return false;
  }

  struct hash_bucket *bucket = HT_BUCKET(htable, hentry);
  struct hash_entry *tail = HT_TAIL(htable);

  htable_link_entry(tail, hentry, bucket);
  htable->num_items++;

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
  struct hash_entry **link = htable_lookup_bucket(htable, bucket, hentry);

  if (*link) {
    *replaced = *link;

    htable_replace_entry(link, hentry);
    return true;
  }

  htable_link_entry(tail, hentry, bucket);
  htable->num_items++;

  return true;
}

struct hash_entry *htable_lookup(hash_table *htable, struct hash_entry *query) {
  assert(htable != NULL);
  assert(query != NULL);

  struct hash_bucket *bucket = HT_BUCKET(htable, query);

  return *htable_lookup_bucket(htable, bucket, query);
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
  struct hash_entry **link = htable_lookup_bucket(htable, bucket, query);
  struct hash_entry *entry = *link;

  if (*link) {
    htable_remove_link(link);
    htable->num_items--;
  }

  return entry;
}

bool htable_delete(hash_table *htable, struct hash_entry *hentry) {
  assert(htable != NULL);
  assert(hentry != NULL);

  struct hash_bucket *bucket = HT_BUCKET_BY_HASHV(htable, hentry);
  struct hash_entry **link = &bucket->entry;

  while (*link && *link != hentry) {
    link = &(*link)->next;
  }

  if (*link) {
    htable_remove_link(link);
    htable->num_items--;

    return true;
  }

  return false;
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
void htable_sort(hash_table *htable, less_ht_fun less) {
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

  tail->ht_next = head->ht_prev = &htable->head;
  HT_SET_HEAD(htable, head);
  HT_SET_TAIL(htable, tail);
}
