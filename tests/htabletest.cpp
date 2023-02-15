#include "gtest/gtest.h"

#include "datastructs/hash_table.h"
#include "datastructs/utils.h"

#include "helper.h"

#include <limits.h>
#include <unordered_map>
#include <unordered_set>

class HashTableTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { htable_free(ht_); }

  template <typename T1, typename T2> void SetHashTable(size_t size = 1) {
    ht_ = htable_create(size, sizeof(T1), sizeof(T2));
    ASSERT_NE(ht_, nullptr);
  }

  hash_table *ht_;
};

TEST(HashTable, Initialization) {
  size_t types[] = {
      sizeof(int),  sizeof(float),    sizeof(double),
      sizeof(char), sizeof(uint16_t),
  };
  size_t types_size = YU_ARRAYSIZE(types);

  for (size_t i = 0; i < types_size; ++i) {
    hash_table *ht = htable_create(Helper::rand(1, 2000), types[i], types[i]);
    ASSERT_NE(ht, nullptr);
    EXPECT_EQ(htable_ksize(ht), types[i]);
    EXPECT_EQ(htable_vsize(ht), types[i]);
    EXPECT_EQ(htable_size(ht), 0);
    htable_free(ht);
  }
}

TEST_F(HashTableTest, InsertRemove) {
  SetHashTable<int, bool>();
  const int num_cases = 1000;
  int nums[num_cases];
  bool t = true;
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    htable_insert(ht_, &val, &t);
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
  SetHashTable<int, bool>();
  const int num_cases = 1000;
  int nums[num_cases];
  bool t = true;
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    htable_insert(ht_, &val, &t);
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
    htable_insert(ht_, &val, &t);
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
  SetHashTable<int, int>();
  int key = 5, val = 7;

  htable_remove(ht_, &key);
  htable_insert(ht_, &key, &val);
  key = 6;
  htable_remove(ht_, &key);
  key = 5;
  htable_remove(ht_, &key);
  htable_remove(ht_, &key);
}

TEST_F(HashTableTest, STLTable) {
  SetHashTable<int, int>();

  std::unordered_map<int, int> stl;
  enum Action {
    Insert,
    Remove,
    Lookup,
  } command;

  const size_t num_commands = 1000000;

  std::vector<int> keys;

  for (size_t i = 0; i < num_commands; ++i) {
    command = static_cast<Action>(Helper::rand(Insert, Lookup));
    EXPECT_EQ(stl.size(), htable_size(ht_));
    if (command == Insert || htable_size(ht_) == 0) {
      int key = Helper::rand(-10000, 10000);
      if (stl.find(key) != stl.end()) {
        ASSERT_TRUE(htable_lookup(ht_, &key) != NULL);
        EXPECT_EQ(stl.at(key), *(int *)htable_lookup(ht_, &key));
      } else {
        keys.push_back(key);
      }
      int val = Helper::rand(INT_MIN, INT_MAX);
      htable_insert(ht_, &key, &val);
      stl[key] = val;

      ASSERT_TRUE(htable_lookup(ht_, &key) != NULL);
      EXPECT_EQ(stl.at(key), *(int *)htable_lookup(ht_, &key));
    } else if (command == Remove) {
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
    } else if (command == Lookup) {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];

      ASSERT_EQ(stl.find(key) != stl.end(), htable_lookup(ht_, &key) != NULL);
      EXPECT_EQ(stl.at(key), *(int *)htable_lookup(ht_, &key));
    }
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
