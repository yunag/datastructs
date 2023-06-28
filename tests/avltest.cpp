#include "gtest/gtest.h"

#include "datastructs/avl_tree.h"

#include "datastructs/functions.h"
#include "helper.h"

#include <algorithm>
#include <array>
#include <climits>
#include <set>

class BSTTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { avl_destroy(avl_); }

  void setAVL() {
    avl_ = avl_create(yu_cmp_i32, free, NULL);
    ASSERT_NE(avl_, nullptr);
  }

  avl_tree *avl_;
};

TEST_F(BSTTest, Case1) {
  setAVL();

  std::vector<int> nums = {
      9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
  };
  std::vector<int> sorted_nums = nums;
  std::sort(sorted_nums.begin(), sorted_nums.end());

  for (int num : nums) {
    avl_insert(avl_, yu_dup_i32(num), NULL);
  }
  int cycles = 0;
  AVL_FOR_EACH(avl_, int *, int *, key, val) {
    EXPECT_EQ(*key, sorted_nums[cycles]) << "Cycles is " << cycles << '\n';
    cycles++;
  }
  ASSERT_EQ(cycles, nums.size());

  cycles = 0;
  AVL_FOR_EACH(avl_, int *, int *, key, val) {
    EXPECT_EQ(*key, sorted_nums[cycles]);
    cycles++;
    if (cycles == 5) {
      break;
    }
  }
  ASSERT_EQ(cycles, 5);
}

TEST_F(BSTTest, Case2) {
  setAVL();
  std::vector<int> nums = {4, 2, 5, 1, 3, 0};
  for (int num : nums) {
    avl_insert(avl_, yu_dup_i32(num), NULL);
  }
  for (int num : nums) {
    ASSERT_TRUE(avl_find(avl_, &num));
  }
}

TEST_F(BSTTest, InsertOnly_ValidAvl) {
  setAVL();

  for (size_t i = 0; i < 10000; ++i) {
    int num = Helper::rand(INT_MIN, INT_MAX);
    avl_insert(avl_, yu_dup_i32(num), NULL);
    ASSERT_TRUE(avl_valid_avl(avl_)) << "Is not a valid AVL Tree";
  }
}

TEST_F(BSTTest, STLSet) {
  setAVL();

  std::set<int> stl;
  enum class Action {
    Insert,
    Remove,
    Find,
  } command;

  const size_t num_actions = 100;

  std::vector<int> keys;

  for (size_t i = 0; i < num_actions; ++i) {
    command = static_cast<Action>(Helper::rand(static_cast<int>(Action::Insert),
                                               static_cast<int>(Action::Find)));
    ASSERT_EQ(stl.size(), avl_size(avl_));
    ASSERT_TRUE(avl_valid_avl(avl_));
    if (stl.empty()) {
      command = Action::Insert;
    }
    switch (command) {
    case Action::Insert: {
      int key = Helper::rand(-10000, 10000);
      if (stl.find(key) != stl.end()) {
        ASSERT_TRUE(avl_find(avl_, &key));
      } else {
        keys.push_back(key);
      }
      stl.insert(key);
      avl_insert(avl_, yu_dup_i32(key), NULL);
      ASSERT_TRUE(avl_find(avl_, &key));
      break;
    }

    case Action::Remove: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];
      ASSERT_TRUE(stl.find(key) != stl.end());
      ASSERT_TRUE(avl_find(avl_, &key));
      stl.erase(key);
      avl_remove(avl_, &key);
      keys.erase(keys.begin() + idx);
      ASSERT_TRUE(avl_find(avl_, &key) == NULL);
      ASSERT_TRUE(stl.find(key) == stl.end());
      break;
    }

    case Action::Find: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];
      ASSERT_EQ(stl.find(key) != stl.end(), avl_find(avl_, &key) != NULL);
      break;
    }
    }
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
