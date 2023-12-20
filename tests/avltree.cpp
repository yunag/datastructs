#include "gtest/gtest.h"

#include "datastructs/avl_tree.h"
#include "datastructs/functions.h"

#include "utils.hpp"

#include <limits>
#include <ostream>
#include <vector>

struct KeyValue {
  KeyValue() {}
  KeyValue(int key_, int val_ = 0) : key(key_), val(val_) {}

  int key;
  int val;
  avl_node ah;
};

int cmpKeyValueNode(const avl_node *a, const avl_node *b) {
  KeyValue *kva = avl_entry(a, KeyValue, ah);
  KeyValue *kvb = avl_entry(b, KeyValue, ah);

  return yu_cmp_i32(&kva->key, &kvb->key);
}

class AVLTree {
public:
  AVLTree() {
    avl_.avl_node = nullptr;
    num_items = 0;
  }

  ~AVLTree() {
    KeyValue *cur, *n;

    avl_postorder_for_each(&avl_, cur, n, ah) { delete cur; }
  }

  void insert(int key, int val = 0) {
    KeyValue *keyValue = new KeyValue(key, val);

    avl_node *node = avl_insert(&keyValue->ah, &avl_, cmpKeyValueNode);
    if (node) {
      delete keyValue;
      keyValue = avl_entry(node, KeyValue, ah);
      keyValue->val = val;
    } else {
      num_items++;
    }
  }

  KeyValue *find(int key) {
    KeyValue query(key);

    avl_node *node = avl_find(&query.ah, &avl_, cmpKeyValueNode);

    return avl_entry_safe(node, KeyValue, ah);
  }

  void remove(int key) {
    KeyValue *node = find(key);

    if (node) {
      num_items--;
      avl_erase(&node->ah, &avl_);
      delete node;
    }
  }

  bool isValid() {
    bool isValid = true;
    isValidAVL_rec(avl_.avl_node, isValid);
    return isValid;
  }

  friend std::ostream &operator<<(std::ostream &os, const AVLTree &avl);

  avl_node *first() { return avl_first(&avl_); }
  avl_node *last() { return avl_last(&avl_); }

  size_t size() { return num_items; }
  avl_root *root() { return &avl_; }

private:
  int isValidAVL_rec(avl_node *node, bool &isValid,
                     int left = std::numeric_limits<int>::min(),
                     int right = std::numeric_limits<int>::max());

private:
  avl_root avl_;
  size_t num_items;
};

void printAVL_impl(std::ostream &os, const std::string &prefix,
                   const avl_node *node, bool isLeft) {
  if (!node) {
    return;
  }
  os << prefix;

  os << (isLeft ? "├──" : "└──");

  KeyValue *keyValue = avl_entry(node, KeyValue, ah);
  os << keyValue->key << '(' << node->height << ')' << std::endl;

  printAVL_impl(os, prefix + (isLeft ? "│   " : "    "), node->left, true);
  printAVL_impl(os, prefix + (isLeft ? "│   " : "    "), node->right, false);
}

std::ostream &operator<<(std::ostream &os, const AVLTree &avl) {
  printAVL_impl(os, "", avl.avl_.avl_node, false);
  return os;
}

int AVLTree::isValidAVL_rec(avl_node *node, bool &isValid, int left,
                            int right) {
  if (!node) {
    return 0;
  }

  KeyValue *keyValue = avl_entry(node, KeyValue, ah);

  /* Check BST properties */
  bool BSTCorrect = keyValue->key > left || keyValue->key < right;
  EXPECT_TRUE(BSTCorrect) << "Violated BST property";

  /* Check height */
  int leftHeight = isValidAVL_rec(node->left, isValid, left, keyValue->key);
  int rightHeight = isValidAVL_rec(node->right, isValid, keyValue->key, right);

  bool heightBalanced = abs(leftHeight - rightHeight) <= 1;
  EXPECT_TRUE(heightBalanced)
      << "Violated AVL height property: abs(leftHeight - rightHeight) <= 1";

  size_t height = 1 + std::max(leftHeight, rightHeight);
  EXPECT_EQ(height, node->height) << "Height mismatch";

  isValid &= heightBalanced & BSTCorrect;

  return height;
}

class AVLTestFixture : public ::testing::Test {
protected:
  void SetUp() override {
    for (int i = 0; i < 10; ++i) {
      /*
       * Tree structure:
       *         3 <-- Root
       *       /   \
       *      /     \
       *     1       7
       *    / \     /  \
       *   0   2   5    8
       *          / \    \
       *         4   6    9
       *
       * postorder: 0, 2, 1, 4, 6, 5, 9, 8, 7, 3
       * inorder: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
       */
      avl_.insert(i, i);
    }
  }

  void TearDown() override {
    auto test_info = ::testing::UnitTest::GetInstance()->current_test_info();

    if (test_info->result()->Failed()) {
      std::cout << avl_;
    }
  }

  AVLTree avl_;
};

TEST(AVLTest, AVLInit_DefaultInitialization_ReturnsInitializedAVL) {
  AVLTree avl;

  size_t avlSize = avl.size();
  avl_node *firstNode = avl.first();
  avl_node *lastNode = avl.last();
  avl_node *root = avl.root()->avl_node;

  EXPECT_EQ(avlSize, 0);
  EXPECT_FALSE(notNull(firstNode));
  EXPECT_FALSE(notNull(lastNode));
  EXPECT_FALSE(notNull(root));
}

TEST(AVLTest, Size_InsertDistinctItems_ReturnsValidSize) {
  AVLTree avl;

  const int numItems = 10;
  for (int i = 0; i < numItems; ++i) {
    avl.insert(i, i);
  }

  size_t avlSize = avl.size();

  ASSERT_EQ(avlSize, numItems);
}

TEST(AVLTest, Size_InsertSameItemMultipleTimes_ReturnsOne) {
  AVLTree avl;

  avl.insert(0);
  avl.insert(0);

  size_t avlSize = avl.size();

  ASSERT_EQ(avlSize, 1);
}

TEST(AVLTest, Size_InsertRemoveInsert_ReturnsOne) {
  AVLTree avl;

  avl.insert(0);
  avl.remove(0);
  avl.insert(0);

  size_t avlSize = avl.size();

  ASSERT_EQ(avlSize, 1);
}

TEST_F(AVLTestFixture, Size_RemoveSingleItem_ReturnsValidSize) {
  size_t avlSizeBefore = avl_.size();

  avl_.remove(0);

  size_t avlSizeAfter = avl_.size();

  ASSERT_EQ(avlSizeBefore, avlSizeAfter + 1);
}

TEST_F(AVLTestFixture, Find_FindExistingItem_ReturnsItem) {
  KeyValue *found = avl_.find(0);

  int foundKey = found->key;
  int foundVal = found->val;

  ASSERT_TRUE(notNull(found));

  EXPECT_EQ(foundKey, 0);
  EXPECT_EQ(foundVal, 0);
}

TEST_F(AVLTestFixture, Find_FindNonExistingItem_ReturnsNull) {
  KeyValue *found = avl_.find(-1);

  ASSERT_FALSE(notNull(found));
}

TEST_F(AVLTestFixture, Find_RemoveExistingItem_ReturnsNull) {
  avl_.remove(0);

  KeyValue *found = avl_.find(0);

  ASSERT_FALSE(notNull(found));
}

TEST_F(AVLTestFixture, Find_RemoveNonExistingItem_ReturnsNull) {
  avl_.remove(-1);

  KeyValue *found = avl_.find(-1);

  ASSERT_FALSE(notNull(found));
}

TEST_F(AVLTestFixture, Insert_MultipleItems_ReturnsValidAVL) {
  bool isValidAVL = avl_.isValid();

  ASSERT_TRUE(isValidAVL);
}

TEST_F(AVLTestFixture, Remove_TwoNodes_ReturnsValidAVL) {
  bool isValidAVL;

  avl_.remove(0);
  isValidAVL = avl_.isValid();
  ASSERT_TRUE(isValidAVL);

  avl_.remove(1);
  isValidAVL = avl_.isValid();
  ASSERT_TRUE(isValidAVL);
}

TEST_F(AVLTestFixture, Remove_MultipleItems_ReturnsValidAVL) {
  bool isValidAVL;

  avl_.remove(1);
  isValidAVL = avl_.isValid();
  ASSERT_TRUE(isValidAVL);

  avl_.remove(2);
  isValidAVL = avl_.isValid();
  ASSERT_TRUE(isValidAVL);

  avl_.remove(3);
  isValidAVL = avl_.isValid();
  ASSERT_TRUE(isValidAVL);
}

TEST_F(AVLTestFixture, Remove_MultipleItemsReversed_ReturnsValidAVL) {
  bool isValidAVL;

  avl_.remove(3);
  isValidAVL = avl_.isValid();
  ASSERT_TRUE(isValidAVL);

  avl_.remove(2);
  isValidAVL = avl_.isValid();
  ASSERT_TRUE(isValidAVL);

  avl_.remove(1);
  isValidAVL = avl_.isValid();
  ASSERT_TRUE(isValidAVL);
}

TEST_F(AVLTestFixture, Remove_TwoItemsTriggersRebalance_ReturnsValidAVL) {
  bool isValidAVL;

  avl_.remove(8);
  isValidAVL = avl_.isValid();
  ASSERT_TRUE(isValidAVL);

  avl_.remove(7);
  isValidAVL = avl_.isValid();
  ASSERT_TRUE(isValidAVL);
}

TEST_F(AVLTestFixture, ForEach_Default_ReturnsValidNumberOfIterations) {
  KeyValue *keyValue;
  avl_root *root = avl_.root();

  size_t numIters = 0;

  avl_for_each(root, keyValue, ah) { numIters++; }

  size_t avlSize = avl_.size();

  ASSERT_EQ(avlSize, numIters);
}

TEST_F(AVLTestFixture,
       ForEach_RemoveMultipleItems_ReturnsValidNumberOfIterations) {
  avl_.remove(0);
  avl_.remove(1);
  avl_.remove(2);

  KeyValue *keyValue;
  avl_root *root = avl_.root();

  size_t numIters = 0;

  avl_for_each(root, keyValue, ah) { numIters++; }

  size_t avlSize = avl_.size();

  ASSERT_EQ(avlSize, numIters);
}

TEST_F(AVLTestFixture,
       PostOrderForEach_Default_ReturnsValidNumberOfIterations) {
  KeyValue *keyValue, *next;
  avl_root *root = avl_.root();

  size_t numIters = 0;

  avl_postorder_for_each(root, keyValue, next, ah) { numIters++; }

  size_t avlSize = avl_.size();

  ASSERT_EQ(avlSize, numIters);
}

TEST_F(AVLTestFixture, PostOrderForEach_Default_ReturnsPostorderTraversal) {
  KeyValue *keyValue, *next;
  avl_root *root = avl_.root();

  std::vector<int> expected = {0, 2, 1, 4, 6, 5, 9, 8, 7, 3};
  std::vector<int> got;

  avl_postorder_for_each(root, keyValue, next, ah) {
    got.push_back(keyValue->key);
  }

  ASSERT_EQ(expected, got);
}

TEST_F(AVLTestFixture, ForEach_Default_ReturnsInorderTraversal) {
  KeyValue *keyValue;
  avl_root *root = avl_.root();

  std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::vector<int> got;

  avl_for_each(root, keyValue, ah) { got.push_back(keyValue->key); }

  ASSERT_EQ(expected, got);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
