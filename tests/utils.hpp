#ifndef YU_UTILS_HPP
#define YU_UTILS_HPP

#include "gtest/gtest.h"

template <typename T>
testing::AssertionResult notNull(T *p) {
  if (p) {
    return testing::AssertionSuccess();
  } else {
    return testing::AssertionFailure() << "Pointer is null";
  }
}

#endif /* !YU_UTILS_HPP */
