#include "gtest/gtest.h"

#include "datastructs/queue.h"

template <typename T> class Queue {
public:
  Queue() { q_ = queue_create(1, sizeof(T)); }
  ~Queue() { queue_destroy(q_); }

  void push(T item) { queue_push(q_, &item); }

  T pop() {
    T item = QUEUE_FRONT(q_, T);
    queue_pop(q_);
    return item;
  }

  T front() { return QUEUE_FRONT(q_, T); }
  T back() { return QUEUE_BACK(q_, T); }

  bool isEmpty() { return queue_empty(q_); }
  bool full() { return queue_full(q_); }

  size_t size() { return queue_size(q_); }
  size_t capacity() { return queue_capacity(q_); }
  size_t itemSize() { return queue_esize(q_); }

private:
  queue *q_;
};

class QueueTestFixture : public ::testing::Test {
protected:
  void SetUp() override {
    for (int i = 0; i < 5; ++i) {
      queue_.push(i);
    }
  }

  void TearDown() override {}

  Queue<int> queue_;
};

TEST(QueueTest, QueueCreate_DefaultInitialization_ReturnsInitializedQueue) {
  Queue<int> queue;

  bool isEmpty = queue.isEmpty();
  bool isFull = queue.full();
  size_t size = queue.size();
  size_t itemSize = queue.itemSize();

  EXPECT_TRUE(isEmpty);
  EXPECT_FALSE(isFull);
  EXPECT_EQ(size, 0);
  EXPECT_EQ(itemSize, sizeof(int));
}

TEST(QueueTest, Size_PushOneItem_ReturnsOne) {
  Queue<int> queue;

  queue.push(0);

  size_t size = queue.size();

  ASSERT_EQ(size, 1);
}

TEST(QueueTest, Size_PushMultipleItems_ReturnsValidSize) {
  Queue<int> queue;

  const int numItems = 10;
  for (int i = 0; i < numItems; ++i) {
    queue.push(i);
  }

  size_t size = queue.size();

  ASSERT_EQ(size, numItems);
}

TEST(QueueTest, Size_PushPopPush_ReturnsOne) {
  Queue<int> queue;

  queue.push(0);
  queue.pop();
  queue.push(0);

  size_t size = queue.size();

  ASSERT_EQ(size, 1);
}

TEST(QueueTest, IsEmpty_PushPopPush_ReturnsFalse) {
  Queue<int> queue;

  queue.push(0);
  queue.pop();
  queue.push(0);

  size_t isEmpty = queue.isEmpty();

  ASSERT_FALSE(isEmpty);
}

TEST(QueueTest, Front_PushOneItem_ReturnsItem) {
  Queue<int> queue;

  queue.push(0);

  int frontItem = queue.front();

  ASSERT_EQ(frontItem, 0);
}

TEST(QueueTest, Back_PushOneItem_ReturnsItem) {
  Queue<int> queue;

  queue.push(0);

  int backItem = queue.back();

  ASSERT_EQ(backItem, 0);
}

TEST_F(QueueTestFixture, Size_PopAllItems_ReturnsZero) {
  while (!queue_.isEmpty()) {
    queue_.pop();
  }

  size_t size = queue_.size();

  ASSERT_EQ(size, 0);
}

TEST_F(QueueTestFixture, Front_Default_ReturnsFrontItem) {
  int frontItem = queue_.front();

  ASSERT_EQ(frontItem, 0);
}

TEST_F(QueueTestFixture, Back_Default_ReturnsBackItem) {
  int backItem = queue_.back();

  ASSERT_EQ(backItem, 4);
}

TEST_F(QueueTestFixture,
       Pop_PushMultipleItemsAndPopThem_ReturnsItemsInCorrectOrder) {

  EXPECT_EQ(queue_.pop(), 0);
  EXPECT_EQ(queue_.pop(), 1);
  EXPECT_EQ(queue_.pop(), 2);
  EXPECT_EQ(queue_.pop(), 3);
  EXPECT_EQ(queue_.pop(), 4);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
