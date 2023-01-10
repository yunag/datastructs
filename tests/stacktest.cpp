#include "gtest/gtest.h"

#include "datastructs/stack.h"
#include "datastructs/utils.h"

#include "helper.h"

#include <limits.h>

class StackTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { stack_free(s_); }

  template <typename T> void SetStack(size_t size = 1) {
    s_ = stack_create(size, sizeof(T));
    ASSERT_NE(s_, nullptr);
  }

  stack *s_;
};

TEST(Stack, Initialization) {
  size_t types[] = {
      sizeof(int),  sizeof(float),    sizeof(double),
      sizeof(char), sizeof(uint16_t),
  };
  size_t types_size = YU_ARRAYSIZE(types);

  for (size_t i = 0; i < types_size; ++i) {
    stack *s = stack_create(Helper::rand(1, 20000), types[i]);
    ASSERT_NE(s, nullptr);
    EXPECT_TRUE(stack_empty(s));
    EXPECT_EQ(stack_esize(s), types[i]);
    EXPECT_EQ(stack_top(s), nullptr);
    stack_free(s);
  }
}

TEST_F(StackTest, PushResize) {
  SetStack<int>(2);
  const size_t num_cases = 200;
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    stack_push(s_, &val);
    EXPECT_FALSE(stack_empty(s_));
    EXPECT_EQ(STACK_TOP(s_, int), val);
    EXPECT_EQ(stack_size(s_), i + 1);
  }
}

TEST_F(StackTest, PushPop) {
  SetStack<int>();
  const size_t num_cases = 10000;
  int nums[num_cases];
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    stack_push(s_, &val);
  }
  for (size_t i = 0; i < num_cases; ++i) {
    EXPECT_EQ(nums[num_cases - i - 1], STACK_TOP(s_, int));
    stack_pop(s_);
    EXPECT_EQ(stack_size(s_), num_cases - i - 1);
  }
  EXPECT_TRUE(stack_empty(s_));
  EXPECT_EQ(stack_top(s_), nullptr);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
