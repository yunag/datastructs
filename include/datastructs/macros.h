#ifndef YU_UTILS_H
#define YU_UTILS_H

#include <stddef.h>

#if defined(__cplusplus) && (__cpp_decltype >= 200707L || _MSC_VER >= 1600)
  #include <type_traits>
  #define yu_typeof(x) std::remove_reference<decltype(x)>::type
#elif defined(__MCST__) || defined(__GNUC__) || defined(__clang__) ||          \
  defined(__chibicc__)
  /* chibicc supports __typeof__ but not __typeof */
  #define yu_typeof(x) __typeof__(x)
#endif

#define YU_BYTE_SWAP(a, b, size)                                               \
  do {                                                                         \
    size_t __size = (size);                                                    \
    char *__a = (a), *__b = (b);                                               \
    do {                                                                       \
      char __tmp = *__a;                                                       \
      *__a++ = *__b;                                                           \
      *__b++ = __tmp;                                                          \
    } while (--__size > 0);                                                    \
  } while (0)

#define YU_UNUSED(param) ((void)(param))

static inline void *yu_container_of_safe(void *ptr, size_t offset) {
  return ptr ? (char *)ptr - offset : NULL;
}

#define YU_CONTAINER_OF(ptr, type, member)                                     \
  ((type *)(void *)(((char *)(ptr)) - offsetof(type, member)))

#define YU_CONTAINER_OF_SAFE(ptr, type, member)                                \
  (type *)yu_container_of_safe(ptr, offsetof(type, member))

#endif  // !YU_UTILS_H
