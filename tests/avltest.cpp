#include "gtest/gtest.h"

#include "datastructs/avl_tree.h"

#include "helper.h"

#include <array>
#include <climits>
#include <set>

class BSTTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { avl_destroy(bst_); }

  void setBST() {
    bst_ = avl_create();
    ASSERT_NE(bst_, nullptr);
  }

  avl_tree *bst_;
};

TEST_F(BSTTest, Insert) {
  setBST();
  const size_t num_cases = 10000;
  std::array<int, num_cases> numbers;
  for (size_t i = 0; i < numbers.size(); ++i) {
    numbers[i] = Helper::rand(INT_MIN, INT_MAX);
    avl_insert(bst_, numbers[i]);
    ASSERT_TRUE(avl_valid_avl(bst_)) << "Is not a valid AVL Tree";
    ASSERT_TRUE(avl_valid_bst(bst_)) << "Is not a valid Binary Search Tree";
  }
}

TEST_F(BSTTest, InsertRemove) {
  setBST();

  const size_t num_cases = 1000;
  std::array<int, num_cases> numbers;
  for (size_t i = 0; i < numbers.size(); ++i) {
    numbers[i] = Helper::rand(INT_MIN, INT_MAX);
    avl_insert(bst_, numbers[i]);
    ASSERT_EQ(avl_size(bst_), i + 1);
  }
  for (size_t i = 0; i < numbers.size(); ++i) {
    ASSERT_TRUE(avl_find(bst_, numbers[i]));
    avl_remove(bst_, numbers[i]);
    ASSERT_FALSE(avl_find(bst_, numbers[i]));
    ASSERT_TRUE(avl_valid_avl(bst_)) << "Is not a valid AVL Tree";
    ASSERT_TRUE(avl_valid_bst(bst_)) << "Is not a valid Binary Search Tree";
  }
}

TEST_F(BSTTest, STLSet) {
  setBST();

  std::set<int> stl;
  enum class Action {
    Insert,
    Remove,
    Find,
  } command;

  const size_t num_commands = 10000;

  std::vector<int> keys;

  for (size_t i = 0; i < num_commands; ++i) {
    command = static_cast<Action>(Helper::rand(static_cast<int>(Action::Insert),
                                               static_cast<int>(Action::Find)));
    ASSERT_EQ(stl.size(), avl_size(bst_));
    ASSERT_TRUE(avl_valid_bst(bst_));
    ASSERT_TRUE(avl_valid_avl(bst_));
    if (stl.empty()) {
      command = Action::Insert;
    }
    switch (command) {
    case Action::Insert: {
      int key = Helper::rand(-10000, 10000);
      if (stl.find(key) != stl.end()) {
        ASSERT_TRUE(avl_find(bst_, key));
      } else {
        keys.push_back(key);
      }
      stl.insert(key);
      avl_insert(bst_, key);
      ASSERT_TRUE(avl_find(bst_, key));
      break;
    }

    case Action::Remove: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];
      ASSERT_TRUE(stl.find(key) != stl.end());
      ASSERT_TRUE(avl_find(bst_, key));
      stl.erase(key);
      avl_remove(bst_, key);
      keys.erase(keys.begin() + idx);
      ASSERT_FALSE(avl_find(bst_, key));
      ASSERT_TRUE(stl.find(key) == stl.end());
      break;
    }

    case Action::Find: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];
      ASSERT_EQ(stl.find(key) != stl.end(), avl_find(bst_, key));
      break;
    }
    }
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
