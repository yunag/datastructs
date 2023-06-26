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

  void setBST() {
    avl_ = avl_create(yu_cmp_i32, NULL, NULL);
    ASSERT_NE(avl_, nullptr);
  }

  avl_tree *avl_;
};

TEST_F(BSTTest, Case1) {
  setBST();
  std::vector<int> nums = {
      9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
  };
  std::vector<int> sorted_nums = nums;
  std::sort(sorted_nums.begin(), sorted_nums.end());
  for (int num : nums) {
    int *ins = reinterpret_cast<int *>(yu_dup_i32(num));
    avl_insert(avl_, ins, ins);
  }
  avl_iterator *it = avl_first(avl_);
  int cycles = 0;
  AVL_FOR_EACH(it, int *, int *, key, val) {
    EXPECT_EQ(*key, sorted_nums[cycles]) << "Cycles is " << cycles << '\n';
    cycles++;
  }
  ASSERT_EQ(cycles, nums.size());
  avl_it_destroy(it);

  it = avl_first(avl_);
  cycles = 0;
  AVL_FOR_EACH(it, int *, int *, key, val) {
    EXPECT_EQ(*key, sorted_nums[cycles]);
    cycles++;
    if (cycles == 5) {
      break;
    }
  }
  avl_it_destroy(it);
  ASSERT_EQ(cycles, 5);
}

TEST_F(BSTTest, Case2) {
  setBST();
  std::vector<int> nums = {4, 2, 5, 1, 3, 0};
  for (int num : nums) {
    int *ins = reinterpret_cast<int *>(yu_dup_i32(num));
    avl_insert(avl_, ins, ins);
  }
  for (int num : nums) {
    ASSERT_TRUE(avl_find(avl_, &num));
  }
}

TEST_F(BSTTest, Insert) {
  setBST();
  const size_t num_cases = 10000;
  std::array<int, num_cases> numbers;

  for (size_t i = 0; i < numbers.size(); ++i) {
    numbers[i] = Helper::rand(INT_MIN, INT_MAX);
    avl_insert(avl_, &numbers[i], &numbers[i]);
    ASSERT_TRUE(avl_valid_avl(avl_)) << "Is not a valid AVL Tree";
    ASSERT_TRUE(avl_find(avl_, &numbers[i]));
  }
}

TEST_F(BSTTest, InsertRemove) {
  setBST();

  const size_t num_cases = 1000;
  std::array<int, num_cases> numbers;
  for (size_t i = 0; i < numbers.size(); ++i) {
    numbers[i] = Helper::rand(INT_MIN, INT_MAX);
    avl_insert(avl_, &numbers[i], &numbers[i]);
    ASSERT_EQ(avl_size(avl_), i + 1);
  }
  for (size_t i = 0; i < numbers.size(); ++i) {
    ASSERT_TRUE(avl_find(avl_, &numbers[i]));
    avl_remove(avl_, &numbers[i]);
    ASSERT_FALSE(avl_find(avl_, &numbers[i]));
    ASSERT_TRUE(avl_valid_avl(avl_)) << "Is not a valid AVL Tree";
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
      int *ins = reinterpret_cast<int *>(yu_dup_i32(key));
      avl_insert(avl_, ins, ins);
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
      ASSERT_FALSE(avl_find(avl_, &key));
      ASSERT_TRUE(stl.find(key) == stl.end());
      break;
    }

    case Action::Find: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];
      ASSERT_EQ(stl.find(key) != stl.end(), avl_find(avl_, &key));
      break;
    }
    }
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
