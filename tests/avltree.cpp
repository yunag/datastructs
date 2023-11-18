#include "gtest/gtest.h"

#include "datastructs/avl_tree.h"
#include "datastructs/functions.h"

#include "helper.h"

#include <algorithm>
#include <climits>
#include <unordered_set>

typedef void (*destroy_tree_fun)(struct avl_root *);

struct kv_node {
  kv_node() {}
  kv_node(int _key, int _val) : key(_key), val(_val) {}

  int key;
  int val;
  struct avl_node node;
};

int valid_avl_rec(avl_node *node, bool &is_valid) {
  if (!node || !is_valid) {
    return 0;
  }
  int lheight = valid_avl_rec(node->left, is_valid);
  int rheight = valid_avl_rec(node->right, is_valid);
  is_valid &= abs(lheight - rheight) <= 1;
  size_t height = 1 + std::max(lheight, rheight);
  EXPECT_EQ(height, node->height);
  return height;
}

bool valid_avl(avl_tree *avl) {
  bool is_valid = true;
  valid_avl_rec(avl->root.avl_node, is_valid);
  return is_valid;
}

bool valid_bst(avl_tree *avl) {
  int prev = INT_MIN;
  kv_node *kv;
  AVL_FOR_EACH(&avl->root, kv, node) {
    if (kv->key < prev) {
      return false;
    }
    prev = kv->key;
  }
  return true;
}

int cmp_kv_node(const avl_node *a, const avl_node *b) {
  kv_node *kva = avl_entry(a, struct kv_node, node);
  kv_node *kvb = avl_entry(b, struct kv_node, node);
  return yu_cmp_i32(&kva->key, &kvb->key);
}

void destroy_kv_tree(struct avl_root *root) {
  kv_node *cur, *n;
  AVL_POSTORDER_FOR_EACH(root, cur, n, node) { delete cur; }
}

class BSTTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {
    destroy_(&avl_->root);
    delete avl_;
  }

  void setAVL(destroy_tree_fun destroy) {
    destroy_ = destroy;

    avl_ = new avl_tree;
    avl_init(avl_, cmp_kv_node);
    ASSERT_NE(avl_, nullptr);
  }

  destroy_tree_fun destroy_;
  avl_tree *avl_ = nullptr;
};

TEST_F(BSTTest, STLSet) {
  setAVL(destroy_kv_tree);

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

    ASSERT_EQ(stl.size(), avl_->num_items);
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
  setAVL(destroy_kv_tree);

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

  kv_node *kv;

  cycles = 0;
  AVL_FOR_EACH(&avl_->root, kv, node) {
    EXPECT_EQ(kv->key, sorted_nums[cycles]) << "Cycles is " << cycles << '\n';
    cycles++;
  }

  ASSERT_EQ(cycles, nums.size());

  cycles = 0;
  AVL_FOR_EACH(&avl_->root, kv, node) {
    EXPECT_EQ(kv->key, sorted_nums[cycles]);
    cycles++;
    if (cycles == 5) {
      break;
    }
  }

  ASSERT_EQ(cycles, 5);
}

TEST_F(BSTTest, Case2) {
  setAVL(destroy_kv_tree);
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
  setAVL(destroy_kv_tree);

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
  setAVL(destroy_kv_tree);

  size_t num_inserts = 1000;
  for (size_t i = 0; i < num_inserts; ++i) {
    int num = Helper::rand_inrange(-2000000, 2000000);
    kv_node *node = new kv_node(num, 0);
    if (avl_insert(avl_, &node->node)) {
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
