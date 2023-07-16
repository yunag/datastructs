#include "gtest/gtest.h"

#include "datastructs/avl_tree.h"
#include "datastructs/functions.h"
#include "datastructs/macros.h"
#include "helper.h"

#include <algorithm>
#include <climits>
#include <map>
#include <unordered_set>

struct kv_node {
  kv_node() {}
  kv_node(int _key, int _val) : key(_key), val(_val) {}

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
  return abs(deviation(node)) <= 1;
}

static bool valid_avl_rec(avl_node *node) {
  if (!node) {
    return true;
  }
  if (!balanced(node)) {
    return false;
  }
  return valid_avl_rec(node->left) && valid_avl_rec(node->right);
}

static bool valid_avl(avl_tree *avl) {
  return valid_avl_rec(avl->root.avl_node);
}

static bool valid_bst(avl_tree *avl) {
  int prev = INT_MIN;
  AVL_FOR_EACH(&avl->root, node) {
    kv_node *kv = avl_entry(node, kv_node, node);
    if (kv->key < prev) {
      return false;
    }
    prev = kv->key;
  }
  return true;
}

int cmp_kv_node(const avl_node *a, const avl_node *b) {
  struct kv_node *kva = avl_entry(a, struct kv_node, node);
  struct kv_node *kvb = avl_entry(b, struct kv_node, node);
  return yu_cmp_i32(&kva->key, &kvb->key);
}

void destroy_kv_node(avl_node *a) {
  struct kv_node *kv = avl_entry(a, struct kv_node, node);
  delete kv;
}

class BSTTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { avl_destroy(avl_); }

  void setAVL(destroy_avl_node_fun destroy = destroy_kv_node) {
    avl_ = avl_create(cmp_kv_node, destroy);
    ASSERT_NE(avl_, nullptr);
  }

  avl_tree *avl_ = nullptr;
};

TEST_F(BSTTest, STLSet) {
  setAVL();

  enum class Action {
    Insert,
    Remove,
    Find,
  } command;

  std::unordered_set<int> stl;
  std::vector<int> keys;
  kv_node query;

  const size_t num_actions = 100000;

  for (size_t i = 0; i < num_actions; ++i) {
    command = static_cast<Action>(Helper::rand_inrange(
        static_cast<int>(Action::Insert), static_cast<int>(Action::Find)));

    ASSERT_EQ(stl.size(), avl_->size);
    ASSERT_TRUE(valid_avl(avl_));
    ASSERT_TRUE(valid_bst(avl_));
    if (stl.empty()) {
      command = Action::Insert;
    }

    switch (command) {
    case Action::Insert: {
      int key = Helper::rand_inrange(-10000, 10000);
      query.key = key;
      if (stl.find(key) != stl.end()) {
        ASSERT_NE(avl_find(avl_, &query.node), nullptr);
      } else {
        ASSERT_EQ(avl_find(avl_, &query.node), nullptr);
        keys.push_back(key);
        stl.insert(key);
        kv_node *toinsert = new kv_node(key, 0);
        avl_insert(avl_, &toinsert->node);
        ASSERT_NE(avl_find(avl_, &query.node), nullptr);
      }
      break;
    }

    case Action::Remove: {
      int idx = Helper::rand_inrange(0, keys.size() - 1);
      int key = keys[idx];
      query.key = key;
      ASSERT_NE(avl_find(avl_, &query.node), nullptr);
      stl.erase(key);
      avl_remove(avl_, &query.node);
      keys.erase(keys.begin() + idx);
      ASSERT_EQ(avl_find(avl_, &query.node), nullptr);
      break;
    }

    case Action::Find: {
      int idx = Helper::rand_inrange(0, keys.size() - 1);
      int key = keys[idx];
      query.key = key;
      if (stl.find(key) != stl.end()) {
        ASSERT_NE(avl_find(avl_, &query.node), nullptr);
      } else {
        ASSERT_EQ(avl_find(avl_, &query.node), nullptr);
      }
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

  int cycles;
  for (int num : nums) {
    kv_node *toinsert = new kv_node(num, 0);
    avl_insert(avl_, &toinsert->node);
  }

  cycles = 0;
  AVL_FOR_EACH(&avl_->root, node) {
    EXPECT_EQ(avl_entry(node, struct kv_node, node)->key, sorted_nums[cycles])
        << "Cycles is " << cycles << '\n';
    cycles++;
  }
  ASSERT_EQ(cycles, nums.size());

  cycles = 0;
  AVL_FOR_EACH(&avl_->root, node) {
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
  kv_node query;
  std::vector<int> nums = {4, 1, 5, 0, 2, 6};
  for (int num : nums) {
    kv_node *toinsert = new kv_node(num, 0);
    avl_insert(avl_, &toinsert->node);
  }

  query.key = 1;
  avl_remove(avl_, &query.node);
  ASSERT_EQ(avl_find(avl_, &query.node), nullptr);
}

TEST_F(BSTTest, Case3) {
  setAVL();
  kv_node query;
  std::vector<int> nums = {4, 2, 5, 1, 3, 0};
  for (int num : nums) {
    kv_node *toinsert = new kv_node(num, 0);
    avl_insert(avl_, &toinsert->node);
  }
  for (int num : nums) {
    query.key = num;
    ASSERT_NE(avl_find(avl_, &query.node), nullptr);
  }
}

TEST_F(BSTTest, InsertOnly_ValidAvl) {
  setAVL();
  size_t num_inserts = 1000;
  for (size_t i = 0; i < num_inserts; ++i) {
    int num = Helper::rand_inrange(-2000000, 2000000);
    kv_node *node = new kv_node(num, 0);
    if (!avl_insert(avl_, &node->node)) {
      delete node;
    }
    ASSERT_TRUE(valid_avl(avl_)) << "It is not a valid AVL Tree";
    ASSERT_TRUE(valid_bst(avl_)) << "It is not a valid BST";
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  srand(time(nullptr));
  return RUN_ALL_TESTS();
}
