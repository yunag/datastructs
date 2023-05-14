#include "gtest/gtest.h"

#include "datastructs/hash_table.h"
#include "datastructs/macros.h"
#include "datastructs/queue.h"

#include "helper.h"

#include <limits.h>
#include <unordered_map>
#include <unordered_set>

class HashTableTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { htable_destroy(ht_); }

  template <typename T1, typename T2>
  void SetHashTable(size_t size = 1, hash_fn hash = NULL,
                    cmp_key_fn cmp_key = NULL, free_fn kfree = NULL,
                    free_fn vfree = NULL) {
    ht_ = htable_create(size, sizeof(T1), sizeof(T2), hash, cmp_key, kfree,
                        vfree);
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
    hash_table *ht = htable_create(Helper::rand(1, 2000), types[i], types[i],
                                   NULL, NULL, NULL, NULL);
    ASSERT_NE(ht, nullptr);
    EXPECT_EQ(htable_ksize(ht), types[i]);
    EXPECT_EQ(htable_vsize(ht), types[i]);
    EXPECT_EQ(htable_size(ht), 0);
    htable_destroy(ht);
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
      htable_insert(ht_, &key, &val);
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

TEST_F(HashTableTest, CustomFree) {
  SetHashTable<int, queue *>(5, NULL, NULL, NULL, [](const void *a) -> void {
    queue_destroy(*(queue **)a);
  });

  const int num_cases = 1000;
  int nums[num_cases];
  for (size_t i = 0; i < num_cases; ++i) {
    int val = Helper::rand(INT_MIN, INT_MAX);
    nums[i] = val;
    queue *q = queue_create(Helper::rand(10, 20), sizeof(double), NULL);
    htable_insert(ht_, &val, &q);
    EXPECT_EQ(htable_size(ht_), i + 1);
  }
  for (size_t i = 0; i < num_cases / 2; ++i) {
    htable_remove(ht_, &nums[i]);
    EXPECT_EQ(htable_lookup(ht_, &nums[i]), nullptr);
    EXPECT_EQ(htable_size(ht_), num_cases - i - 1);
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
