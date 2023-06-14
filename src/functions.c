#include "datastructs/functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define FNV_PRIME 0x100000001b3
#define FNV_OFFSET 0xcbf29ce484222325UL
#define FNHASHDEF(T, postfix)                                                  \
  uint64_t yu_hash_##postfix(const void *key) {                                \
    const unsigned char *bytes = key;                                          \
    uint64_t hashv = FNV_OFFSET;                                               \
    for (size_t i = 0; i < sizeof(T); ++i) {                                   \
      hashv ^= bytes[i];                                                       \
      hashv *= FNV_PRIME;                                                      \
    }                                                                          \
    return hashv;                                                              \
  }
#define FNCMPDEF(T, postfix)                                                   \
  bool yu_cmp_##postfix(const void *a, const void *b) {                        \
    return *(T *)a == *(T *)b;                                                 \
  }
#define FNDUPDEF(T, postfix)                                                   \
  void *yu_dup_##postfix(T value) {                                            \
    T *val = malloc(sizeof(T));                                                \
    if (val) {                                                                 \
      *val = value;                                                            \
    }                                                                          \
    return val;                                                                \
  }

#define TYPED_FUNCTIONS(Type, postfix)                                         \
  FNDUPDEF(Type, postfix)                                                      \
  FNHASHDEF(Type, postfix)                                                     \
  FNCMPDEF(Type, postfix)

TYPED_FUNCTIONS(int64_t, i64)
TYPED_FUNCTIONS(int32_t, i32)
TYPED_FUNCTIONS(int16_t, i16)
TYPED_FUNCTIONS(int8_t, i8)
TYPED_FUNCTIONS(uint64_t, u64)
TYPED_FUNCTIONS(uint32_t, u32)
TYPED_FUNCTIONS(uint16_t, u16)
TYPED_FUNCTIONS(uint8_t, u8)
TYPED_FUNCTIONS(double, double)
TYPED_FUNCTIONS(float, float)
TYPED_FUNCTIONS(void *, ptr)

uint64_t yu_hash_str(const void *str) {
  const unsigned char *bytes = (const unsigned char *)str;
  uint64_t hashv = FNV_OFFSET;
  while (*bytes) {
    hashv ^= *bytes++;
    hashv *= FNV_PRIME;
  }
  return hashv;
}

bool yu_cmp_str(const void *a, const void *b) { return !strcmp(a, b); }

void *yu_dup_str(const char *value) {
  size_t slen = strlen(value);
  char *str = malloc(slen + 1);
  if (str) {
    memcpy(str, value, slen + 1);
  }
  return str;
}
