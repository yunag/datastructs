#ifndef YU_FUNCITONS_H
#define YU_FUNCITONS_H

#include "macros.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void free_placeholder(void *ptr) { YU_UNUSED(ptr); }

void *yu_dup_str(const char *value);
void *yu_dup_float(float value);
void *yu_dup_double(double value);

void *yu_dup_i64(int64_t value);
void *yu_dup_i32(int32_t value);
void *yu_dup_i16(int16_t value);
void *yu_dup_i8(int8_t value);
void *yu_dup_u64(uint64_t value);
void *yu_dup_u32(uint32_t value);
void *yu_dup_u16(uint16_t value);
void *yu_dup_u8(uint8_t value);

uint64_t yu_hash_str(const void *value);
uint64_t yu_hash_float(const void *value);
uint64_t yu_hash_double(const void *value);

uint64_t yu_hash_i64(const void *value);
uint64_t yu_hash_i32(const void *value);
uint64_t yu_hash_i16(const void *value);
uint64_t yu_hash_i8(const void *value);
uint64_t yu_hash_u64(const void *value);
uint64_t yu_hash_u32(const void *value);
uint64_t yu_hash_u16(const void *value);
uint64_t yu_hash_u8(const void *value);

bool yu_cmp_str(const void *a, const void *b);
bool yu_cmp_float(const void *a, const void *b);
bool yu_cmp_double(const void *a, const void *b);

bool yu_cmp_i64(const void *a, const void *b);
bool yu_cmp_i32(const void *a, const void *b);
bool yu_cmp_i16(const void *a, const void *b);
bool yu_cmp_i8(const void *a, const void *b);
bool yu_cmp_u64(const void *a, const void *b);
bool yu_cmp_u32(const void *a, const void *b);
bool yu_cmp_u16(const void *a, const void *b);
bool yu_cmp_u8(const void *a, const void *b);

#ifdef __cplusplus
}
#endif

#endif /* !YU_FUNCITONS_H */
