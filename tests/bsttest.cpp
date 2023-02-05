#include "gtest/gtest.h"

#include "datastructs/bst.h"

#include "helper.h"

#include <array>
#include <climits>
#include <set>

class BSTTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { bst_free(bst_); }

  void setBST() {
    bst_ = bst_create();
    ASSERT_NE(bst_, nullptr);
  }

  bst *bst_;
};

TEST_F(BSTTest, Insert) {
  setBST();
  const size_t num_cases = 10000;
  std::array<int, num_cases> numbers;
  for (size_t i = 0; i < numbers.size(); ++i) {
    numbers[i] = Helper::rand(INT_MIN, INT_MAX);
    bst_insert(bst_, numbers[i]);
    ASSERT_TRUE(bst_valid_avl(bst_)) << "Is not a valid AVL Tree";
    ASSERT_TRUE(bst_valid_bst(bst_)) << "Is not a valid Binary Search Tree";
  }
}

TEST_F(BSTTest, InsertRemove) {
  setBST();

  const size_t num_cases = 1000;
  std::array<int, num_cases> numbers;
  for (size_t i = 0; i < numbers.size(); ++i) {
    numbers[i] = Helper::rand(INT_MIN, INT_MAX);
    bst_insert(bst_, numbers[i]);
    ASSERT_EQ(bst_size(bst_), i + 1);
  }
  for (size_t i = 0; i < numbers.size(); ++i) {
    ASSERT_TRUE(bst_find(bst_, numbers[i]));
    bst_remove(bst_, numbers[i]);
    ASSERT_FALSE(bst_find(bst_, numbers[i]));
    ASSERT_TRUE(bst_valid_avl(bst_)) << "Is not a valid AVL Tree";
    ASSERT_TRUE(bst_valid_bst(bst_)) << "Is not a valid Binary Search Tree";
  }
}

TEST_F(BSTTest, STLSet) {
  setBST();

  std::set<int> stl;
  enum Action {
    Insert,
    Remove,
    Find,
  } command;

  const size_t num_commands = 10000;

  std::vector<int> keys;

  for (size_t i = 0; i < num_commands; ++i) {
    command = static_cast<Action>(Helper::rand(Insert, Find));
    ASSERT_EQ(stl.size(), bst_size(bst_));
    ASSERT_TRUE(bst_valid_bst(bst_));
    ASSERT_TRUE(bst_valid_avl(bst_));
    if (command == Insert || bst_size(bst_) == 0) {
      int key = Helper::rand(-10000, 10000);
      if (stl.find(key) != stl.end()) {
        ASSERT_TRUE(bst_find(bst_, key));
      } else {
        keys.push_back(key);
      }
      stl.insert(key);
      bst_insert(bst_, key);
      ASSERT_TRUE(bst_find(bst_, key));
    } else if (command == Remove) {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];
      ASSERT_TRUE(stl.find(key) != stl.end());
      ASSERT_TRUE(bst_find(bst_, key));
      stl.erase(key);
      bst_remove(bst_, key);
      keys.erase(keys.begin() + idx);
      ASSERT_FALSE(bst_find(bst_, key));
      ASSERT_TRUE(stl.find(key) == stl.end());
    } else if (command == Find) {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];
      ASSERT_EQ(stl.find(key) != stl.end(), bst_find(bst_, key));
    }
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
