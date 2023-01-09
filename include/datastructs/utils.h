#ifndef YU_UTILS_H
#define YU_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#define yu_arraysize(_ARR) (sizeof(_ARR) / sizeof(_ARR[0]))

#define yu_min(_VAL1, _VAL2)                                                   \
  ({                                                                           \
    __typeof__(_VAL1) _ARG1 = (_VAL1);                                         \
    __typeof__(_VAL2) _ARG2 = (_VAL2);                                         \
    _ARG1 < _ARG2 ? _ARG1 : _ARG2;                                             \
  })

#define yu_max(_VAL1, _VAL2)                                                   \
  ({                                                                           \
    __typeof__(_VAL1) _ARG1 = (_VAL1);                                         \
    __typeof__(_VAL2) _ARG2 = (_VAL2);                                         \
    _ARG1 > _ARG2 ? _ARG1 : _ARG2;                                             \
  })

#define yu_abs(_VAL)                                                           \
  ({                                                                           \
    __typeof__(_VAL) _ARG = (_VAL);                                            \
    _ARG < 0 ? -1 * _ARG : _ARG;                                               \
  })

#define yu_swap(_PVAL1, _PVAL2)                                                \
  ({                                                                           \
    __typeof__(*(_PVAL1)) temp = *(_PVAL1);                                    \
    *(_PVAL1) = *(_PVAL2);                                                     \
    *(_PVAL2) = temp;                                                          \
  })

#ifndef NDEBUG
#include <stdio.h>
#define yu_log_error(msg)                                                      \
  fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, msg)
#else
#define yu_log_error(msg)
#endif // !DEBUG

#define yu_byte_swap(a, b, size)                                               \
  do {                                                                         \
    size_t __size = (size);                                                    \
    char *__a = (a), *__b = (b);                                               \
    do {                                                                       \
      char __tmp = *__a;                                                       \
      *__a++ = *__b;                                                           \
      *__b++ = __tmp;                                                          \
    } while (--__size > 0);                                                    \
  } while (0)

#ifdef __cplusplus
}
#endif

#endif // !YU_UTILS_H
