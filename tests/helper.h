#ifndef HELPER_H
#define HELPER_H

#include <cassert>
#include <cstdlib>
#include <random>

namespace Helper {

static inline double rand_inrange(double low, double high) {
  assert(low <= high);
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(low, high);
  return dis(gen);
}

static inline int rand_inrange(int low, int high) {
  assert(low <= high);
  double scale = (double)rand() / (RAND_MAX + 1.0);
  return (1LL + high - low) * scale + low;
}

} // namespace Helper

#endif // !HELPER_H
