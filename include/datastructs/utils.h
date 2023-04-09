#ifndef YU_UTILS_H
#define YU_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif
#define YU_ARRAYSIZE(base) (sizeof(base) / sizeof(base[0]))

#define YU_MIN(a, b)                                                           \
  ({                                                                           \
    __typeof__(a) __a = (a);                                                   \
    __typeof__(b) __b = (b);                                                   \
    __a < __b ? __a : __b;                                                     \
  })

#define YU_MAX(a, b)                                                           \
  ({                                                                           \
    __typeof__(a) __a = (a);                                                   \
    __typeof__(b) __b = (b);                                                   \
    __a > __b ? __a : __b;                                                     \
  })

#define YU_ABS(a)                                                              \
  ({                                                                           \
    __typeof__(a) __a = (a);                                                   \
    __a < 0 ? -1 * __a : __a;                                                  \
  })

#define YU_SWAP(a, b)                                                          \
  ({                                                                           \
    __typeof__(*(a)) __temp = *(a);                                            \
    *(a) = *(b);                                                               \
    *(b) = __temp;                                                             \
  })

#define YU_STR(x) YU_STR2(x)
#define YU_STR2(x) #x

#ifndef NDEBUG
#include <stdio.h>
#define YU_LOG_ERROR(...)                                                      \
  fprintf(stderr, __FILE__ ":" YU_STR(__LINE__) ": " __VA_ARGS__)
#else
#define YU_LOG_ERROR(msg)
#endif // !DEBUG

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

#ifdef __cplusplus
}
#endif

#endif // !YU_UTILS_H
