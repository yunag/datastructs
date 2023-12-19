#include "gtest/gtest.h"

#include "datastructs/macros.h"
#include "datastructs/queue.h"

#include "helper.h"

#include <climits>
#include <queue>

class QueueTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { queue_destroy(q_); }

  template <typename T> void SetQueue(size_t size = 1) {
    q_ = queue_create(size, sizeof(T));
    ASSERT_NE(q_, nullptr);
  }

  queue *q_;
};

TEST(Queue, Initialization) {
  std::vector<size_t> types = {
      sizeof(int),  sizeof(float),    sizeof(double),
      sizeof(char), sizeof(uint16_t),
  };

  for (size_t i = 0; i < types.size(); ++i) {
    queue *q = queue_create(Helper::rand_inrange(1, 20000), types[i]);
    ASSERT_NE(q, nullptr);
    ASSERT_TRUE(queue_empty(q));
    ASSERT_EQ(queue_esize(q), types[i]);
    ASSERT_EQ(queue_front(q), nullptr);
    ASSERT_EQ(queue_back(q), nullptr);
    queue_destroy(q);
  }
}

TEST_F(QueueTest, PushResize) {
  SetQueue<int>(2);
  const size_t num_cases = 200;
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand_inrange(INT_MIN, INT_MAX);
    QUEUE_PUSH(q_, val);
    ASSERT_FALSE(queue_empty(q_));
    ASSERT_EQ(QUEUE_BACK(q_, int), val);
    ASSERT_EQ(queue_size(q_), i + 1);
  }
}

TEST_F(QueueTest, PushPop) {
  SetQueue<int>();
  const size_t num_cases = 10000;
  int nums[num_cases];
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand_inrange(INT_MIN, INT_MAX);
    nums[i] = val;
    QUEUE_PUSH(q_, val);
  }
  for (size_t i = 0; i < num_cases; ++i) {
    ASSERT_EQ(nums[i], QUEUE_FRONT(q_, int));
    queue_pop(q_);
  }
  ASSERT_TRUE(queue_empty(q_));
  ASSERT_EQ(queue_front(q_), nullptr);
  ASSERT_EQ(queue_back(q_), nullptr);
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
    command = static_cast<Action>(Helper::rand_inrange(
        static_cast<double>(Action::Push), static_cast<double>(Action::Back)));
    ASSERT_EQ(q.empty(), queue_empty(q_));
    ASSERT_EQ(q.size(), queue_size(q_));
    if (q.empty()) {
      command = Action::Push;
    }

    switch (command) {
    case Action::Push: {
      double val = Helper::rand_inrange(INT_MIN, INT_MAX);
      q.push(val);
      queue_push(q_, &val);
      ASSERT_EQ(q.front(), QUEUE_FRONT(q_, double));
      ASSERT_EQ(q.back(), QUEUE_BACK(q_, double));
      break;
    }

    case Action::Pop: {
      ASSERT_EQ(q.front(), QUEUE_FRONT(q_, double));
      ASSERT_EQ(q.back(), QUEUE_BACK(q_, double));
      q.pop();
      queue_pop(q_);
      break;
    }

    case Action::Front: {
      ASSERT_EQ(q.front(), QUEUE_FRONT(q_, double));
      break;
    }

    case Action::Back: {
      ASSERT_EQ(q.back(), QUEUE_BACK(q_, double));
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
