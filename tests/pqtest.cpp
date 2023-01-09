#include "gtest/gtest.h"

#include "datastructs/priority_queue.h"
#include "datastructs/utils.h"

#include "helper.h"

#include <limits.h>
#include <stdlib.h>

template <typename T> int cmp_great(const void *pa, const void *pb) {
  T a = *(T *)pa;
  T b = *(T *)pb;
  if (a > b) {
    return 1;
  }
  if (a < b) {
    return -1;
  }
  return 0;
}

template <typename T> int cmp_less(const void *pa, const void *pb) {
  return -cmp_great<T>(pa, pb);
}

class PriorityQueueTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { pq_free(pq_); }

  template <typename T>
  void SetPQueue(size_t size = 1, cmp_fn cmp = cmp_great<T>) {
    pq_ = pq_create(size, sizeof(T), cmp);
    ASSERT_NE(pq_, nullptr);
  }

  priority_queue *pq_;
};

TEST(PriorityQueue, Initialization) {
  size_t types[] = {
      sizeof(int),  sizeof(float),    sizeof(double),
      sizeof(char), sizeof(uint16_t),
  };
  size_t types_size = yu_arraysize(types);

  for (size_t i = 0; i < types_size; ++i) {
    priority_queue *pq =
        pq_create(Helper::rand(1, 10000), types[i], cmp_great<int>);
    ASSERT_NE(pq, nullptr);
    EXPECT_TRUE(pq_empty(pq));
    EXPECT_EQ(pq_esize(pq), types[i]);
    EXPECT_EQ(pq_top(pq), nullptr);
    pq_free(pq);
  }
}

TEST_F(PriorityQueueTest, PushResize) {
  SetPQueue<int>();
  const size_t num_cases = 200;
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    pq_push(pq_, &val);
    EXPECT_FALSE(pq_empty(pq_));
    EXPECT_EQ(pq_size(pq_), i + 1);
  }
}

TEST_F(PriorityQueueTest, Greater) {
  SetPQueue<int>();
  const size_t num_cases = 400;
  int nums[num_cases];

  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    pq_push(pq_, &val);
  }
  qsort(nums, num_cases, sizeof(*nums), cmp_great<int>);

  for (size_t i = 0; i < num_cases; ++i) {
    EXPECT_EQ(nums[i], PQ_TOP(pq_, int));
    pq_pop(pq_);
    EXPECT_EQ(pq_size(pq_), num_cases - i - 1);
  }
  EXPECT_TRUE(pq_empty(pq_));
  EXPECT_EQ(pq_top(pq_), nullptr);
}

TEST_F(PriorityQueueTest, Lesser) {
  SetPQueue<double>(1, cmp_less<double>);
  const size_t num_cases = 400;
  double nums[num_cases];

  for (size_t i = 0; i < num_cases; ++i) {
    double val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    pq_push(pq_, &val);
  }
  qsort(nums, num_cases, sizeof(*nums), cmp_less<double>);

  for (size_t i = 0; i < num_cases; ++i) {
    EXPECT_EQ(nums[i], PQ_TOP(pq_, double));
    pq_pop(pq_);
    EXPECT_EQ(pq_size(pq_), num_cases - i - 1);
  }
  EXPECT_TRUE(pq_empty(pq_));
}

TEST_F(PriorityQueueTest, Case1) {
  SetPQueue<double>();

  PQ_PUSH(pq_, double, 5);
  EXPECT_EQ(PQ_TOP(pq_, double), 5);
  EXPECT_FALSE(pq_empty(pq_));

  PQ_PUSH(pq_, double, 7);
  EXPECT_EQ(PQ_TOP(pq_, double), 5);

  pq_pop(pq_);
  EXPECT_EQ(PQ_TOP(pq_, double), 7);

  pq_pop(pq_);
  EXPECT_TRUE(pq_empty(pq_));

  PQ_PUSH(pq_, double, 120);
  EXPECT_EQ(PQ_TOP(pq_, double), 120);
  EXPECT_EQ(pq_size(pq_), 1);

  PQ_PUSH(pq_, double, 79);
  EXPECT_EQ(PQ_TOP(pq_, double), 79);
  EXPECT_EQ(pq_size(pq_), 2);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
