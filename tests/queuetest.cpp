#include "gtest/gtest.h"

#include "datastructs/queue.h"
#include "datastructs/utils.h"

#include "helper.h"

#include <limits.h>

class StackTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { queue_free(q_); }

  template <typename T> void SetStack(size_t size = 1) {
    q_ = queue_create(size, sizeof(T));
    ASSERT_NE(q_, nullptr);
  }

  queue *q_;
};

TEST(Queue, Initialization) {
  size_t types[] = {
      sizeof(int),  sizeof(float),    sizeof(double),
      sizeof(char), sizeof(uint16_t),
  };
  size_t types_size = yu_arraysize(types);

  for (size_t i = 0; i < types_size; ++i) {
    queue *q = queue_create(Helper::rand(1, 20000), types[i]);
    ASSERT_NE(q, nullptr);
    EXPECT_TRUE(queue_empty(q));
    EXPECT_EQ(queue_esize(q), types[i]);
    EXPECT_EQ(queue_front(q), nullptr);
    EXPECT_EQ(queue_back(q), nullptr);
    queue_free(q);
  }
}

TEST_F(StackTest, PushResize) {
  SetStack<int>(2);
  const size_t num_cases = 200;
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    QUEUE_PUSH(q_, int, val);
    EXPECT_FALSE(queue_empty(q_));
    EXPECT_EQ(QUEUE_BACK(q_, int), val);
    EXPECT_EQ(queue_size(q_), i + 1);
  }
}

TEST_F(StackTest, PushPop) {
  SetStack<int>();
  const size_t num_cases = 10000;
  int nums[num_cases];
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    QUEUE_PUSH(q_, int, val);
  }
  for (size_t i = 0; i < num_cases; ++i) {
    EXPECT_EQ(nums[i], QUEUE_FRONT(q_, int));
    queue_pop(q_);
    EXPECT_EQ(queue_size(q_), num_cases - i - 1);
  }
  EXPECT_TRUE(queue_empty(q_));
  EXPECT_EQ(queue_front(q_), nullptr);
  EXPECT_EQ(queue_back(q_), nullptr);
}

TEST_F(StackTest, Case1) {
  SetStack<double>();
  QUEUE_PUSH(q_, double, 5);
  EXPECT_EQ(QUEUE_FRONT(q_, double), 5);
  EXPECT_EQ(QUEUE_BACK(q_, double), 5);
  EXPECT_TRUE(queue_full(q_));

  QUEUE_PUSH(q_, double, 7);
  EXPECT_EQ(QUEUE_FRONT(q_, double), 5);
  EXPECT_EQ(QUEUE_BACK(q_, double), 7);

  queue_pop(q_);
  EXPECT_EQ(QUEUE_FRONT(q_, double), 7);
  EXPECT_EQ(QUEUE_BACK(q_, double), 7);

  queue_pop(q_);
  EXPECT_TRUE(queue_empty(q_));

  QUEUE_PUSH(q_, double, 120);
  EXPECT_EQ(QUEUE_FRONT(q_, double), 120);
  EXPECT_EQ(QUEUE_BACK(q_, double), 120);
  EXPECT_EQ(queue_size(q_), 1);

  QUEUE_PUSH(q_, double, 79);
  EXPECT_EQ(QUEUE_FRONT(q_, double), 120);
  EXPECT_EQ(QUEUE_BACK(q_, double), 79);
  EXPECT_EQ(queue_size(q_), 2);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
