#include "datastructs/queue.h"
#include "gtest/gtest.h"

TEST(Queue, Initialization) {
  queue *q = queue_create(5, sizeof(int));
  ASSERT_EQ(q, NULL);
  EXPECT_EQ(queue_size(q), 5);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
