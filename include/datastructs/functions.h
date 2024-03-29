#ifndef YU_FUNCITONS_H
#define YU_FUNCITONS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t yu_hash_bern(const void *key, size_t size);
size_t yu_hash_fnv1a(const void *key, size_t size);

#define FUNCTION_DECL(type, postfix)                                           \
  size_t yu_hash_##postfix(type key);                                          \
  int yu_cmp_##postfix(const void *a, const void *b);

FUNCTION_DECL(int64_t, i64)
FUNCTION_DECL(int32_t, i32)
FUNCTION_DECL(int16_t, i16)
FUNCTION_DECL(int8_t, i8)
FUNCTION_DECL(uint64_t, u64)
FUNCTION_DECL(uint32_t, u32)
FUNCTION_DECL(uint16_t, u16)
FUNCTION_DECL(uint8_t, u8)
FUNCTION_DECL(double, double)
FUNCTION_DECL(float, float)
FUNCTION_DECL(void *, ptr)

char *yu_dup_str(const char *str);
size_t yu_hash_str(const char *str);

#undef FUNCTION_DECL

#ifdef __cplusplus
}
#endif

#endif /* !YU_FUNCITONS_H */
