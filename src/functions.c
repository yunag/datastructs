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
  void *yu_dup_##postfix(const T value) {                                      \
    T *val = malloc(sizeof(T));                                                \
    *val = value;                                                              \
    return val;                                                                \
  }

FNDUPDEF(int64_t, i64)
FNDUPDEF(int32_t, i32)
FNDUPDEF(int16_t, i16)
FNDUPDEF(int8_t, i8)
FNDUPDEF(uint64_t, u64)
FNDUPDEF(uint32_t, u32)
FNDUPDEF(uint16_t, u16)
FNDUPDEF(uint8_t, u8)
FNDUPDEF(float, float)
FNDUPDEF(double, double)

FNHASHDEF(int64_t, i64)
FNHASHDEF(int32_t, i32)
FNHASHDEF(int16_t, i16)
FNHASHDEF(int8_t, i8)
FNHASHDEF(uint64_t, u64)
FNHASHDEF(uint32_t, u32)
FNHASHDEF(uint16_t, u16)
FNHASHDEF(uint8_t, u8)
FNHASHDEF(float, float)
FNHASHDEF(double, double)

FNCMPDEF(int64_t, i64)
FNCMPDEF(int32_t, i32)
FNCMPDEF(int16_t, i16)
FNCMPDEF(int8_t, i8)
FNCMPDEF(uint64_t, u64)
FNCMPDEF(uint32_t, u32)
FNCMPDEF(float, float)
FNCMPDEF(double, double)

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
