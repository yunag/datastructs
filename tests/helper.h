#ifndef HELPER_H
#define HELPER_H

#include <cassert>
#include <random>

namespace Helper {

static inline double rand(double low, double high) {
  assert(low <= high);
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(low, high);
  return dis(gen);
}

static inline int rand(int low, int high) {
  assert(low <= high);
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(low, high);
  return dis(gen);
}

} // namespace Helper

#endif // !HELPER_H
