#include "gtest/gtest.h"

#include "datastructs/macros.h"
#include "datastructs/queue.h"

#include "helper.h"

#include <limits.h>
#include <queue>

class QueueTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { queue_destroy(q_); }

  template <typename T> void SetQueue(size_t size = 1) {
    q_ = queue_create(size, sizeof(T), NULL);
    ASSERT_NE(q_, nullptr);
  }

  queue *q_;
};

TEST(Queue, Initialization) {
  size_t types[] = {
      sizeof(int),  sizeof(float),    sizeof(double),
      sizeof(char), sizeof(uint16_t),
  };
  size_t types_size = YU_ARRAYSIZE(types);

  for (size_t i = 0; i < types_size; ++i) {
    queue *q = queue_create(Helper::rand(1, 20000), types[i], NULL);
    ASSERT_NE(q, nullptr);
    EXPECT_TRUE(queue_empty(q));
    EXPECT_EQ(queue_esize(q), types[i]);
    EXPECT_EQ(queue_front(q), nullptr);
    EXPECT_EQ(queue_back(q), nullptr);
    queue_destroy(q);
  }
}

TEST_F(QueueTest, PushResize) {
  SetQueue<int>(2);
  const size_t num_cases = 200;
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    QUEUE_PUSH(q_, val);
    EXPECT_FALSE(queue_empty(q_));
    EXPECT_EQ(QUEUE_BACK(q_, int), val);
    EXPECT_EQ(queue_size(q_), i + 1);
  }
}

TEST_F(QueueTest, PushPop) {
  SetQueue<int>();
  const size_t num_cases = 10000;
  int nums[num_cases];
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    QUEUE_PUSH(q_, val);
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

TEST_F(QueueTest, STLQueue) {
  SetQueue<double>(1);

  enum class Action {
    Push,
    Pop,
    Front,
    Back,
  } command;

  std::queue<double> q;
  const size_t num_commands = 100000;
  for (size_t i = 0; i < num_commands; ++i) {
    command = static_cast<Action>(Helper::rand(
        static_cast<double>(Action::Push), static_cast<double>(Action::Back)));
    EXPECT_EQ(q.empty(), queue_empty(q_));
    EXPECT_EQ(q.size(), queue_size(q_));
    if (q.empty()) {
      command = Action::Push;
    }

    switch (command) {
    case Action::Push: {
      double val = Helper::rand(INT_MIN, INT_MAX);
      q.push(val);
      queue_push(q_, &val);
      EXPECT_EQ(q.front(), QUEUE_FRONT(q_, double));
      EXPECT_EQ(q.back(), QUEUE_BACK(q_, double));
      break;
    }

    case Action::Pop: {
      EXPECT_EQ(q.front(), QUEUE_FRONT(q_, double));
      EXPECT_EQ(q.back(), QUEUE_BACK(q_, double));
      q.pop();
      queue_pop(q_);
      break;
    }

    case Action::Front: {
      EXPECT_EQ(q.front(), QUEUE_FRONT(q_, double));
      break;
    }

    case Action::Back: {
      EXPECT_EQ(q.back(), QUEUE_BACK(q_, double));
      break;
    }
    }
  }
}

TEST_F(QueueTest, Case1) {
  SetQueue<double>();
  QUEUE_PUSH(q_, 5.0);
  EXPECT_EQ(QUEUE_FRONT(q_, double), 5);
  EXPECT_EQ(QUEUE_BACK(q_, double), 5);
  EXPECT_TRUE(queue_full(q_));

  QUEUE_PUSH(q_, 7.0);
  EXPECT_EQ(QUEUE_FRONT(q_, double), 5);
  EXPECT_EQ(QUEUE_BACK(q_, double), 7);

  queue_pop(q_);
  EXPECT_EQ(QUEUE_FRONT(q_, double), 7);
  EXPECT_EQ(QUEUE_BACK(q_, double), 7);

  queue_pop(q_);
  EXPECT_TRUE(queue_empty(q_));

  QUEUE_PUSH(q_, 120.0);
  EXPECT_EQ(QUEUE_FRONT(q_, double), 120);
  EXPECT_EQ(QUEUE_BACK(q_, double), 120);
  EXPECT_EQ(queue_size(q_), 1);

  QUEUE_PUSH(q_, 79.0);
  EXPECT_EQ(QUEUE_FRONT(q_, double), 120);
  EXPECT_EQ(QUEUE_BACK(q_, double), 79);
  EXPECT_EQ(queue_size(q_), 2);
}

TEST_F(QueueTest, Case2) {
  SetQueue<double>(5);
  QUEUE_PUSH(q_, 5.0);
  QUEUE_PUSH(q_, 5.0);
  QUEUE_PUSH(q_, 1.0);
  queue_pop(q_);
  queue_pop(q_);

  QUEUE_PUSH(q_, 2.0);
  QUEUE_PUSH(q_, 3.0);
  QUEUE_PUSH(q_, 4.0);
  QUEUE_PUSH(q_, 5.0);
  QUEUE_PUSH(q_, 6.0);
  EXPECT_EQ(queue_size(q_), 6);
  for (size_t i = 0, qsize = queue_size(q_); i < qsize - 1; ++i) {
    EXPECT_EQ(QUEUE_FRONT(q_, double), i + 1);
    queue_pop(q_);
  }
  EXPECT_EQ(QUEUE_FRONT(q_, double), 6);
  EXPECT_FALSE(queue_empty(q_));
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
