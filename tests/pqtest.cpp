#include "gtest/gtest.h"

#include "datastructs/macros.h"
#include "datastructs/priority_queue.h"

#include "helper.h"

#include <limits.h>
#include <queue>
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
  void TearDown() override { pq_destroy(pq_); }

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
  size_t types_size = YU_ARRAYSIZE(types);

  for (size_t i = 0; i < types_size; ++i) {
    priority_queue *pq =
        pq_create(Helper::rand(1, 10000), types[i], cmp_great<int>);
    ASSERT_NE(pq, nullptr);
    EXPECT_TRUE(pq_empty(pq));
    EXPECT_EQ(pq_esize(pq), types[i]);
    EXPECT_EQ(pq_top(pq), nullptr);
    pq_destroy(pq);
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
    ASSERT_EQ(nums[i], PQ_TOP(pq_, int));
    pq_pop(pq_);
    ASSERT_EQ(pq_size(pq_), num_cases - i - 1);
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
    ASSERT_EQ(nums[i], PQ_TOP(pq_, double));
    pq_pop(pq_);
    ASSERT_EQ(pq_size(pq_), num_cases - i - 1);
  }
  EXPECT_TRUE(pq_empty(pq_));
}

template <typename PriorityQueueType>
void stl_priority_queue(priority_queue *pq_, PriorityQueueType pq) {
  enum class Action {
    Push,
    Pop,
    Top,
  } command;

  const size_t num_commands = 100000;
  for (size_t i = 0; i < num_commands; ++i) {
    command = static_cast<Action>(Helper::rand(static_cast<int>(Action::Push),
                                               static_cast<int>(Action::Top)));
    ASSERT_EQ(pq.empty(), pq_empty(pq_));
    ASSERT_EQ(pq.size(), pq_size(pq_));
    if (pq_empty(pq_)) {
      command = Action::Push;
    }
    switch (command) {

    case Action::Push: {
      double val = Helper::rand(INT_MIN, INT_MAX);
      pq.push(val);
      pq_push(pq_, &val);
      ASSERT_EQ(pq.top(), PQ_TOP(pq_, double));
      break;
    }

    case Action::Pop: {
      ASSERT_EQ(pq.top(), PQ_TOP(pq_, double));
      pq.pop();
      pq_pop(pq_);
      break;
    }

    case Action::Top: {
      ASSERT_EQ(pq.top(), PQ_TOP(pq_, double));
      break;
    }
    }
  }
}

TEST_F(PriorityQueueTest, STLPQueueLesser) {
  SetPQueue<int64_t>(1, cmp_less<double>);
  std::priority_queue<double> pq;
  stl_priority_queue(pq_, pq);
}

TEST_F(PriorityQueueTest, STLPQueueGreater) {
  SetPQueue<double>(1);
  std::priority_queue<double, std::vector<double>, std::greater<double>> pq;
  stl_priority_queue(pq_, pq);
}

TEST_F(PriorityQueueTest, Case1) {
  SetPQueue<double>();

  PQ_PUSH(pq_, 5.0);
  EXPECT_EQ(PQ_TOP(pq_, double), 5.0);
  EXPECT_FALSE(pq_empty(pq_));

  PQ_PUSH(pq_, 7.0);
  EXPECT_EQ(PQ_TOP(pq_, double), 5.0);

  pq_pop(pq_);
  EXPECT_EQ(PQ_TOP(pq_, double), 7);

  pq_pop(pq_);
  EXPECT_TRUE(pq_empty(pq_));

  PQ_PUSH(pq_, 120.0);
  EXPECT_EQ(PQ_TOP(pq_, double), 120);
  EXPECT_EQ(pq_size(pq_), 1);

  PQ_PUSH(pq_, 79.0);
  EXPECT_EQ(PQ_TOP(pq_, double), 79);
  EXPECT_EQ(pq_size(pq_), 2);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
