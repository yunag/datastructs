#include "gtest/gtest.h"

#include <algorithm>
#include <climits>
#include <vector>

#include "datastructs/priority_queue.h"

template <typename T>
bool cmp_less(const void *pa, const void *pb) {
  return *(T *)pa < *(T *)pb;
}

template <typename T>
bool cmp_great(const void *pa, const void *pb) {
  return !cmp_less<T>(pa, pb);
}

template <typename T>
class PriorityQueue {
public:
  PriorityQueue() { pq_ = pq_create(1, sizeof(T), cmp_less<T>); }

  ~PriorityQueue() { pq_destroy(pq_); }

  void push(T item) { pq_push(pq_, &item); }

  T pop() {
    T item = PQ_TOP(pq_, T);
    pq_pop(pq_);
    return item;
  }

  T top() { return PQ_TOP(pq_, T); }

  bool isEmpty() { return pq_empty(pq_); }

  size_t size() { return pq_size(pq_); }

  size_t itemSize() { return pq_esize(pq_); }

private:
  priority_queue *pq_;
};

class PQTestFixture : public ::testing::Test {
protected:
  void SetUp() override {
    for (int i = 0; i < 5; ++i) {
      pq_.push(i);
    }
  }

  void TearDown() override {}

  PriorityQueue<int> pq_;
};

TEST(PriorityQueueTest, PQCreate_DefaultInitialization_ReturnsInitializedPQ) {
  PriorityQueue<int> pq;

  bool isEmpty = pq.isEmpty();
  size_t size = pq.size();
  size_t itemSize = pq.itemSize();

  EXPECT_TRUE(isEmpty);
  EXPECT_EQ(size, 0);
  EXPECT_EQ(itemSize, sizeof(int));
}

TEST(PriorityQueueTest, Size_PushOneItem_ReturnsOne) {
  PriorityQueue<int> pq;

  pq.push(0);

  size_t size = pq.size();

  ASSERT_EQ(size, 1);
}

TEST(PriorityQueueTest, Size_PushMultipleItems_ReturnsValidSize) {
  PriorityQueue<int> pq;

  const int numItems = 10;
  for (int i = 0; i < numItems; ++i) {
    pq.push(i);
  }

  size_t size = pq.size();

  ASSERT_EQ(size, numItems);
}

TEST(PriorityQueueTest, Size_PushPopPush_ReturnsOne) {
  PriorityQueue<int> pq;

  pq.push(0);
  pq.pop();
  pq.push(0);

  size_t size = pq.size();

  ASSERT_EQ(size, 1);
}

TEST(PriorityQueueTest, IsEmpty_PushPopPush_ReturnsFalse) {
  PriorityQueue<int> pq;

  pq.push(0);
  pq.pop();
  pq.push(0);

  size_t isEmpty = pq.isEmpty();

  ASSERT_FALSE(isEmpty);
}

TEST(PriorityQueueTest, Top_PushOneItem_ReturnsItem) {
  PriorityQueue<int> pq;

  pq.push(0);

  int topItem = pq.top();

  ASSERT_EQ(topItem, 0);
}

TEST_F(PQTestFixture, Size_PopAllItems_ReturnsZero) {
  while (!pq_.isEmpty()) {
    pq_.pop();
  }

  size_t size = pq_.size();

  ASSERT_EQ(size, 0);
}

TEST_F(PQTestFixture, Top_Default_ReturnsFrontItem) {
  int topItem = pq_.top();

  ASSERT_EQ(topItem, 0);
}

class PQSortTest : public ::testing::TestWithParam<std::vector<int>> {};

INSTANTIATE_TEST_SUITE_P(
  Instantiation, PQSortTest,
  ::testing::Values(std::vector<int>{8, 7, 6, 5, 4, 3, 2, 1, 0},
                    std::vector<int>{7, 5, 1, 0, 9, -5},

                    std::vector<int>{0, 1, 2, 3, 4},
                    std::vector<int>{7, 8, 9, 10, 4, 0, 1, 2},
                    std::vector<int>{-9, -10, -8, -6, -7, 5},
                    std::vector<int>{1, 0, 5, 4, 200, 555, 88, 300, 10, 27,
                                     -500, -27, INT_MAX, INT_MIN}

                    ));

TEST_P(PQSortTest,
       PopAndPush_PushMultipleItemsAndPopThem_ReturnsItemsInSortedOrder) {
  PriorityQueue<int> pq;

  std::vector<int> input = GetParam();
  std::vector<int> popSequence;

  for (int num : input) {
    pq.push(num);
  }

  while (!pq.isEmpty()) {
    popSequence.push_back(pq.pop());
  }

  ASSERT_TRUE(std::is_sorted(popSequence.begin(), popSequence.end()));
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
