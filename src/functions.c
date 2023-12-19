#include "datastructs/functions.h"
#include "datastructs/memory.h"

#include <stdbool.h>
#include <string.h>

#define FNV_PRIME 0x100000001b3
#define FNV_OFFSET 0xcbf29ce484222325UL

size_t yu_hash_bern(const void *key, size_t size) {
  const unsigned char *bytes = key;
  size_t hash = 5381;
  for (size_t i = 0; i < size; ++i) {
    hash = (hash << 5) + hash + bytes[i];
  }
  return hash;
}

size_t yu_hash_fnv1a(const void *key, size_t size) {
  const unsigned char *bytes = (const unsigned char *)&key;
  size_t hashv = FNV_OFFSET;
  for (size_t i = 0; i < size; ++i) {
    hashv ^= bytes[i];
    hashv *= FNV_PRIME;
  }
  return hashv;
}

#define FNHASHDEF(type, postfix)                                               \
  size_t yu_hash_##postfix(type key) {                                         \
    return yu_hash_fnv1a(&key, sizeof(key));                                   \
  }

#define FNCMPDEF(type, postfix)                                                \
  int yu_cmp_##postfix(const void *a, const void *b) {                         \
    if (*(type *)a > *(type *)b) {                                             \
      return 1;                                                                \
    }                                                                          \
    if (*(type *)a < *(type *)b) {                                             \
      return -1;                                                               \
    }                                                                          \
    return 0;                                                                  \
  }

#define TYPED_FUNCTIONS(Type, postfix)                                         \
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

uint64_t yu_hash_str(const char *str) {
  const unsigned char *bytes = (const unsigned char *)str;
  uint64_t hashv = FNV_OFFSET;
  while (*bytes) {
    hashv ^= *bytes++;
    hashv *= FNV_PRIME;
  }
  return hashv;
}

char *yu_dup_str(const char *s) {
  size_t slen = strlen(s);
  char *str = yu_malloc(slen + 1);
  if (str) {
    memcpy(str, s, slen + 1);
  }
  return str;
}
