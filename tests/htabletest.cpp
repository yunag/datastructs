#include "gtest/gtest.h"

#include "datastructs/functions.h"
#include "datastructs/hash_table.h"
#include "datastructs/macros.h"

#include "helper.h"

#include <limits.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>

class HashTableTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { htable_destroy(ht_); }

  void SetHashTable(hash_fn hash, cmp_key_fn cmp_key, size_t size = 1,
                    free_fn kfree = free, free_fn vfree = free) {
    ht_ = htable_create(size, hash, cmp_key, kfree, vfree);
    ASSERT_NE(ht_, nullptr);
  }

  hash_table *ht_;
};

TEST_F(HashTableTest, InsertRemove) {
  SetHashTable(yu_hash_i32, yu_cmp_i32);
  const int num_cases = 1000;
  int nums[num_cases];
  bool t = true;
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    htable_insert(ht_, yu_dup_i32(val), yu_dup_u8(t));
    EXPECT_EQ(htable_size(ht_), i + 1);
    EXPECT_TRUE(*(bool *)htable_lookup(ht_, &nums[i]));
  }
  for (size_t i = 0; i < num_cases; ++i) {
    EXPECT_TRUE(*(bool *)htable_lookup(ht_, &nums[i]));
    htable_remove(ht_, &nums[i]);
    EXPECT_EQ(htable_lookup(ht_, &nums[i]), nullptr);
    EXPECT_EQ(htable_size(ht_), num_cases - i - 1);
  }
}

TEST_F(HashTableTest, InsertRemoveBackwards) {
  SetHashTable(yu_hash_i32, yu_cmp_i32);
  const int num_cases = 1000;
  int nums[num_cases];
  bool t = true;
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    htable_insert(ht_, yu_dup_i32(val), yu_dup_u8(t));
    EXPECT_EQ(htable_size(ht_), i + 1);
    EXPECT_TRUE(*(bool *)htable_lookup(ht_, &nums[i]));
  }
  for (int64_t i = num_cases - 1; i >= 0; --i) {
    EXPECT_TRUE(*(bool *)htable_lookup(ht_, &nums[i]));
    htable_remove(ht_, &nums[i]);
    EXPECT_EQ(htable_lookup(ht_, &nums[i]), nullptr);
    EXPECT_EQ(htable_size(ht_), i);
  }
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    htable_insert(ht_, yu_dup_i32(val), yu_dup_u8(t));
    EXPECT_EQ(htable_size(ht_), i + 1);
    EXPECT_TRUE(*(bool *)htable_lookup(ht_, &nums[i]));
  }
  for (int64_t i = num_cases - 1; i >= 0; --i) {
    EXPECT_TRUE(*(bool *)htable_lookup(ht_, &nums[i]));
    htable_remove(ht_, &nums[i]);
    EXPECT_EQ(htable_lookup(ht_, &nums[i]), nullptr);
    EXPECT_EQ(htable_size(ht_), i);
  }
}

TEST_F(HashTableTest, RemoveNotExistent) {
  SetHashTable(yu_hash_i32, yu_cmp_i32);
  int key = 5, val = 7;

  htable_remove(ht_, &key);
  htable_insert(ht_, yu_dup_i32(key), yu_dup_i32(val));
  key = 6;
  htable_remove(ht_, &key);
  key = 5;
  htable_remove(ht_, &key);
  htable_remove(ht_, &key);
}

TEST_F(HashTableTest, STLTable) {
  SetHashTable(yu_hash_i32, yu_cmp_i32);

  std::unordered_map<int, int> stl;
  enum class Action {
    Insert,
    Remove,
    Lookup,
  } command;

  const size_t num_commands = 1000000;

  std::vector<int> keys;

  for (size_t i = 0; i < num_commands; ++i) {
    command =
        static_cast<Action>(Helper::rand(static_cast<double>(Action::Insert),
                                         static_cast<double>(Action::Lookup)));
    EXPECT_EQ(stl.size(), htable_size(ht_));

    if (stl.empty()) {
      command = Action::Insert;
    }
    switch (command) {
    case Action::Insert: {
      int key = Helper::rand(-10000, 10000);
      if (stl.find(key) != stl.end()) {
        ASSERT_TRUE(htable_lookup(ht_, &key) != NULL);
        EXPECT_EQ(stl.at(key), *(int *)htable_lookup(ht_, &key));
      } else {
        keys.push_back(key);
      }
      int val = Helper::rand(INT_MIN, INT_MAX);
      htable_insert(ht_, yu_dup_i32(key), yu_dup_i32(val));
      stl[key] = val;

      ASSERT_TRUE(htable_lookup(ht_, &key) != NULL);
      EXPECT_EQ(stl.at(key), *(int *)htable_lookup(ht_, &key));
      break;
    }

    case Action::Remove: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];

      ASSERT_TRUE(stl.find(key) != stl.end());
      ASSERT_TRUE(htable_lookup(ht_, &key) != NULL);
      EXPECT_EQ(stl.at(key), *(int *)htable_lookup(ht_, &key));

      stl.erase(key);
      htable_remove(ht_, &key);
      keys.erase(keys.begin() + idx);

      EXPECT_TRUE(stl.find(key) == stl.end());
      EXPECT_TRUE(htable_lookup(ht_, &key) == NULL);
      break;
    }
    case Action::Lookup: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];

      ASSERT_EQ(stl.find(key) != stl.end(), htable_lookup(ht_, &key) != NULL);
      EXPECT_EQ(stl.at(key), *(int *)htable_lookup(ht_, &key));
      break;
    }
    }
  }
}

TEST_F(HashTableTest, Case1) {
  SetHashTable(yu_hash_i32, yu_cmp_i32);
  std::unordered_map<int, int> unord_map;

  std::vector<std::pair<int, int>> kvalues = {
      {8, 9}, {8, 10}, {9, 25}, {9, 30}};
  for (const auto &kv : kvalues) {
    htable_insert(ht_, yu_dup_i32(kv.first), yu_dup_i32(kv.second));
    unord_map[kv.first] = kv.second;
  }
  HT_FOR_EACH(ht_, int *, int *, key, val) { ASSERT_EQ(*val, unord_map[*key]); }
}

TEST_F(HashTableTest, Strings) {
  SetHashTable(yu_hash_str, yu_cmp_str);

  std::vector<std::pair<const char *, int>> kvalues = {
      {"Jacob", 9},   {"Banana", 10}, {"Banana", 25},
      {"Haruhi", 30}, {"Apple", 31},
  };
  for (const auto &kv : kvalues) {
    htable_insert(ht_, yu_dup_str(kv.first), yu_dup_i32(kv.second));
  }
  ASSERT_TRUE(htable_lookup(ht_, "Banana") != NULL);
  ASSERT_TRUE(htable_lookup(ht_, "Apple") != NULL);
  ASSERT_TRUE(htable_lookup(ht_, "Haruhi") != NULL);
  ASSERT_TRUE(htable_lookup(ht_, "Jacob") != NULL);
  ASSERT_TRUE(htable_lookup(ht_, "Orange") == NULL);

  EXPECT_EQ(*HT_FIND(ht_, "Banana", int *), 25);
  EXPECT_EQ(*HT_FIND(ht_, "Apple", int *), 31);
  EXPECT_EQ(*HT_FIND(ht_, "Haruhi", int *), 30);
  EXPECT_EQ(*HT_FIND(ht_, "Jacob", int *), 9);

  EXPECT_TRUE(htable_remove(ht_, "Jacob"));
  EXPECT_TRUE(htable_lookup(ht_, "Jacob") == NULL);
  EXPECT_TRUE(htable_lookup(ht_, "Banana") != NULL);

  EXPECT_FALSE(htable_remove(ht_, "Moscow"));
  EXPECT_TRUE(htable_remove(ht_, "Banana"));
  EXPECT_TRUE(htable_lookup(ht_, "Banana") == NULL);

  EXPECT_EQ(htable_size(ht_), 2);

  EXPECT_EQ(*HT_FIND(ht_, "Apple", int *), 31);
  EXPECT_EQ(*HT_FIND(ht_, "Haruhi", int *), 30);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
