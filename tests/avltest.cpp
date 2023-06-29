#include "gtest/gtest.h"

#include "datastructs/avl_tree.h"

#include "datastructs/functions.h"
#include "helper.h"

#include <algorithm>
#include <array>
#include <set>

struct kv_node {
  int key;
  int val;
  struct avl_node node;
};

static inline int bheight(struct avl_node *node) {
  return !node ? 0 : node->height;
}

static inline int deviation(struct avl_node *node) {
  return bheight(node->left) - bheight(node->right);
}

static inline bool balanced(struct avl_node *node) {
  return YU_ABS(deviation(node)) <= 1;
}

static bool valid_avl(avl_tree *avl) {
  AVL_FOR_EACH(avl, node) {
    if (!balanced(node)) {
      return false;
    }
  }
  return true;
}

int cmp_kv_node(const avl_node *a, const avl_node *b) {
  struct kv_node *kva = avl_entry(a, struct kv_node, node);
  struct kv_node *kvb = avl_entry(b, struct kv_node, node);
  return yu_cmp_i32(&kva->key, &kvb->key);
}

class BSTTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { avl_destroy(avl_); }

  void setAVL() {
    avl_ = avl_create(cmp_kv_node);
    ASSERT_NE(avl_, nullptr);
  }

  avl_tree *avl_;
};

TEST_F(BSTTest, STLSet) {
  setAVL();

  std::set<int> stl;
  enum class Action {
    Insert,
    Remove,
    Find,
  } command;

  const size_t num_actions = 10000;

  std::vector<int> keys;

  for (size_t i = 0; i < num_actions; ++i) {
    command = static_cast<Action>(Helper::rand(static_cast<int>(Action::Insert),
                                               static_cast<int>(Action::Find)));
    ASSERT_EQ(stl.size(), avl_size(avl_));
    ASSERT_TRUE(valid_avl(avl_));
    if (stl.empty()) {
      command = Action::Insert;
    }
    switch (command) {
    case Action::Insert: {
      int key = Helper::rand(-10000, 10000);
      struct kv_node query = {.key = key};
      if (stl.find(key) != stl.end()) {
        ASSERT_TRUE(avl_find(avl_, &query.node));
      } else {
        keys.push_back(key);
      }
      stl.insert(key);
      struct kv_node *toinsert =
          reinterpret_cast<struct kv_node *>(malloc(sizeof(*toinsert)));
      toinsert->key = key;
      toinsert->val = 0;
      avl_insert(avl_, &toinsert->node);
      ASSERT_TRUE(avl_find(avl_, &query.node));
      break;
    }

    case Action::Remove: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];
      ASSERT_TRUE(stl.find(key) != stl.end());
      struct kv_node query = {.key = key};
      ASSERT_TRUE(avl_find(avl_, &query.node) != NULL);
      stl.erase(key);
      avl_remove(avl_, &query.node);
      keys.erase(keys.begin() + idx);
      ASSERT_TRUE(avl_find(avl_, &query.node) == NULL);
      ASSERT_TRUE(stl.find(key) == stl.end());
      break;
    }

    case Action::Find: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];
      struct kv_node query = {.key = key};
      ASSERT_EQ(stl.find(key) != stl.end(),
                avl_find(avl_, &query.node) != NULL);
      break;
    }
    }
  }
}

TEST_F(BSTTest, Case1) {
  setAVL();

  std::vector<int> nums = {
      9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
  };
  std::vector<int> sorted_nums = nums;
  std::sort(sorted_nums.begin(), sorted_nums.end());

  for (int num : nums) {
    struct kv_node *toinsert =
        reinterpret_cast<struct kv_node *>(malloc(sizeof(*toinsert)));
    toinsert->key = num;
    toinsert->val = 0;
    avl_insert(avl_, &toinsert->node);
  }
  int cycles = 0;
  AVL_FOR_EACH(avl_, node) {
    EXPECT_EQ(avl_entry(node, struct kv_node, node)->key, sorted_nums[cycles])
        << "Cycles is " << cycles << '\n';
    cycles++;
  }
  ASSERT_EQ(cycles, nums.size());

  cycles = 0;
  AVL_FOR_EACH(avl_, node) {
    EXPECT_EQ(avl_entry(node, struct kv_node, node)->key, sorted_nums[cycles]);
    cycles++;
    if (cycles == 5) {
      break;
    }
  }
  ASSERT_EQ(cycles, 5);
}

TEST_F(BSTTest, Case2) {
  setAVL();
  std::vector<int> nums = {4, 1, 5, 0, 2, 6};
  for (int num : nums) {
    struct kv_node *toinsert =
        reinterpret_cast<struct kv_node *>(malloc(sizeof(*toinsert)));
    toinsert->key = num;
    toinsert->val = 0;
    avl_insert(avl_, &toinsert->node);
  }

  struct kv_node query = {.key = 1};
  avl_remove(avl_, &query.node);
  ASSERT_TRUE(avl_find(avl_, &query.node) == NULL);
}

TEST_F(BSTTest, Case3) {
  setAVL();
  std::vector<int> nums = {4, 2, 5, 1, 3, 0};
  for (int num : nums) {
    struct kv_node *toinsert =
        reinterpret_cast<struct kv_node *>(malloc(sizeof(*toinsert)));
    toinsert->key = num;
    toinsert->val = 0;
    avl_insert(avl_, &toinsert->node);
  }
  for (int num : nums) {
    struct kv_node query = {.key = num};
    ASSERT_TRUE(avl_find(avl_, &query.node) != NULL);
  }
}

TEST_F(BSTTest, InsertOnly_ValidAvl) {
  setAVL();

  for (size_t i = 0; i < 10000; ++i) {
    int num = Helper::rand(INT_MIN, INT_MAX);
    struct kv_node *node =
        reinterpret_cast<struct kv_node *>(malloc(sizeof(*node)));
    node->key = num;
    node->val = 0;
    avl_insert(avl_, &node->node);
    ASSERT_TRUE(valid_avl(avl_)) << "Is not a valid AVL Tree";
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
