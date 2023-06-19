#ifndef YU_TYPES_H
#define YU_TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef void (*free_fn)(void *);
typedef int (*cmp_fn)(const void *, const void *);

struct key_value {
  void *key;
  void *val;
};

#endif // !YU_TYPES_H
