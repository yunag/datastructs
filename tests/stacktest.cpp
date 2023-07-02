#include "gtest/gtest.h"

#include "datastructs/macros.h"
#include "datastructs/stack.h"

#include "helper.h"

#include <limits.h>
#include <stack>

class StackTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { stack_destroy(s_); }

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
    stack *s = stack_create(Helper::rand_inrange(1, 20000), types[i]);
    ASSERT_NE(s, nullptr);
    EXPECT_TRUE(stack_empty(s));
    EXPECT_EQ(stack_esize(s), types[i]);
    EXPECT_EQ(stack_top(s), nullptr);
    stack_destroy(s);
  }
}

TEST_F(StackTest, PushResize) {
  SetStack<int>(2);
  const size_t num_cases = 200;
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand_inrange(INT_MIN, INT_MAX);
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
    int val = Helper::rand_inrange(INT_MIN, INT_MAX);
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

TEST_F(StackTest, STLStack) {
  SetStack<double>(1);
  enum class Action {
    Push,
    Pop,
    Top,
  } command;

  std::stack<double> s;
  const size_t num_commands = 100000;
  for (size_t i = 0; i < num_commands; ++i) {
    command = static_cast<Action>(Helper::rand_inrange(
        static_cast<int>(Action::Push), static_cast<int>(Action::Top)));
    EXPECT_EQ(s.empty(), stack_empty(s_));
    EXPECT_EQ(s.size(), stack_size(s_));

    if (s.empty()) {
      command = Action::Push;
    }
    switch (command) {
    case Action::Push: {
      double val = Helper::rand_inrange(INT_MIN, INT_MAX);
      s.push(val);
      stack_push(s_, &val);
      EXPECT_EQ(s.top(), STACK_TOP(s_, double));
      break;
    }

    case Action::Pop: {
      EXPECT_EQ(s.top(), STACK_TOP(s_, double));
      s.pop();
      stack_pop(s_);
      break;
    }

    case Action::Top: {
      EXPECT_EQ(s.top(), STACK_TOP(s_, double));
      break;
    }
    }
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
