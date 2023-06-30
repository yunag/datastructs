#include "gtest/gtest.h"

#include "datastructs/functions.h"
#include "datastructs/hash_table.h"

#include "datastructs/macros.h"
#include "helper.h"

#include <limits.h>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>

class HashTableTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { htable_destroy(ht_); }

  void SetHashTable(hash_ht_entries_fn hash, cmp_ht_entries_fn cmp_key,
                    size_t size = 1) {
    ht_ = htable_create(size, hash, cmp_key);
    ASSERT_NE(ht_, nullptr);
  }

  hash_table *ht_;
};

typedef struct ht_key_value {
  int key;
  int val;
  hash_entry ht_entry;
} ht_key_value;

uint64_t hash_ht_key_value(const hash_entry *a) {
  ht_key_value *kva = ht_entry(a, ht_key_value, ht_entry);
  return yu_hash_i32(&kva->key);
}

int cmp_ht_key_value(const hash_entry *a, const hash_entry *b) {
  ht_key_value *kva = ht_entry(a, ht_key_value, ht_entry);
  ht_key_value *kvb = ht_entry(b, ht_key_value, ht_entry);
  return yu_cmp_i32(&kva->key, &kvb->key);
}

TEST_F(HashTableTest, RemoveNotExistent) {
  SetHashTable(hash_ht_key_value, cmp_ht_key_value);

  ht_key_value query;
  int key = 5;

  query.key = 5;
  htable_remove(ht_, &query.ht_entry);

  ht_key_value *toinsert = new ht_key_value;
  toinsert->key = key;

  htable_insert(ht_, &toinsert->ht_entry);

  query.key = 6;
  htable_remove(ht_, &query.ht_entry);

  query.key = 5;
  htable_remove(ht_, &query.ht_entry);
  htable_remove(ht_, &query.ht_entry);
}

TEST_F(HashTableTest, STLTable) {
  SetHashTable(hash_ht_key_value, cmp_ht_key_value);

  std::unordered_map<int, int> stl;
  ht_key_value query;

  enum class Action {
    Insert,
    Remove,
    Lookup,
  } command;

  const size_t num_commands = 1000000;

  std::vector<int> keys;

  for (size_t i = 0; i < num_commands; ++i) {
    command = static_cast<Action>(Helper::rand(
        static_cast<int>(Action::Insert), static_cast<int>(Action::Lookup)));
    EXPECT_EQ(stl.size(), htable_size(ht_));

    if (stl.empty()) {
      command = Action::Insert;
    }
    switch (command) {
    case Action::Insert: {
      int key = Helper::rand(-10000, 10000);
      if (stl.find(key) != stl.end()) {
        query.key = key;
        ASSERT_TRUE(htable_lookup(ht_, &query.ht_entry) != NULL);

        ht_key_value *kv = ht_entry(htable_lookup(ht_, &query.ht_entry),
                                    ht_key_value, ht_entry);
        EXPECT_EQ(stl.at(key), kv->val);
      } else {
        keys.push_back(key);
      }
      int val = Helper::rand(INT_MIN, INT_MAX);
      ht_key_value *toinsert = new ht_key_value;
      toinsert->key = key;
      toinsert->val = val;
      htable_insert(ht_, &toinsert->ht_entry);
      stl[key] = val;

      query.key = key;
      ASSERT_TRUE(htable_lookup(ht_, &query.ht_entry) != NULL);

      ht_key_value *kv =
          ht_entry(htable_lookup(ht_, &query.ht_entry), ht_key_value, ht_entry);
      EXPECT_EQ(stl.at(key), kv->val);
      break;
    }

    case Action::Remove: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];

      ASSERT_TRUE(stl.find(key) != stl.end());

      query.key = key;
      ASSERT_TRUE(htable_lookup(ht_, &query.ht_entry) != NULL);

      ht_key_value *kv =
          ht_entry(htable_lookup(ht_, &query.ht_entry), ht_key_value, ht_entry);
      EXPECT_EQ(stl.at(key), kv->val);

      stl.erase(key);
      htable_remove(ht_, &query.ht_entry);
      keys.erase(keys.begin() + idx);

      EXPECT_TRUE(stl.find(key) == stl.end());
      EXPECT_TRUE(htable_lookup(ht_, &query.ht_entry) == NULL);
      break;
    }
    case Action::Lookup: {
      int idx = Helper::rand(0, keys.size() - 1);
      int key = keys[idx];

      query.key = key;
      ASSERT_EQ(stl.find(key) != stl.end(),
                htable_lookup(ht_, &query.ht_entry) != NULL);

      ht_key_value *kv =
          ht_entry(htable_lookup(ht_, &query.ht_entry), ht_key_value, ht_entry);
      EXPECT_EQ(stl.at(key), kv->val);
      break;
    }
    }
  }
}

TEST_F(HashTableTest, Case1) {
  SetHashTable(hash_ht_key_value, cmp_ht_key_value);
  std::unordered_map<int, int> unord_map;

  std::vector<std::pair<int, int>> kvalues = {
      {8, 9}, {8, 10}, {9, 25}, {9, 30}};
  for (const auto &kv : kvalues) {
    ht_key_value *toinsert = new ht_key_value;
    toinsert->key = kv.first;
    toinsert->val = kv.second;

    htable_insert(ht_, &toinsert->ht_entry);
    unord_map[kv.first] = kv.second;
  }
  int cycles = 0;
  HT_FOR_EACH(ht_, entry) {
    ht_key_value *kv = ht_entry(entry, ht_key_value, ht_entry);
    cycles++;
    ASSERT_EQ(kv->val, unord_map[kv->key]);
  }
  ASSERT_EQ(cycles, 2);

  cycles = 0;
  HT_FOR_EACH(ht_, entry) {
    ht_key_value *kv = ht_entry(entry, ht_key_value, ht_entry);
    cycles++;
    ASSERT_EQ(kv->val, unord_map[kv->key]);
    if (cycles == 1) {
      break;
    }
  }
  ASSERT_EQ(cycles, 1);
}

typedef struct ht_str_entry {
  char *key;
  int val;
  struct hash_entry ht_entry;
} ht_str_entry;

uint64_t hash_ht_str_entry(const hash_entry *a) {
  ht_str_entry *kva = ht_entry(a, ht_str_entry, ht_entry);
  return yu_hash_str(kva->key);
}

int cmp_ht_str_entry(const hash_entry *a, const hash_entry *b) {
  ht_str_entry *kva = ht_entry(a, ht_str_entry, ht_entry);
  ht_str_entry *kvb = ht_entry(b, ht_str_entry, ht_entry);
  return yu_cmp_str(kva->key, kvb->key);
}

TEST_F(HashTableTest, Strings) {
  SetHashTable(hash_ht_str_entry, cmp_ht_str_entry);

  std::vector<std::pair<const char *, int>> kvalues = {
      {"Jacob", 9},   {"Banana", 10}, {"Banana", 25},
      {"Haruhi", 30}, {"Apple", 31},
  };

  ht_str_entry query;

  for (const auto &kv : kvalues) {
    ht_str_entry *toinsert = new ht_str_entry;
    toinsert->key = (char *)kv.first;
    toinsert->val = kv.second;
    htable_insert(ht_, &toinsert->ht_entry);
  }

  char buffer[256];
  query.key = buffer;

  strcpy(buffer, "Banana");
  ASSERT_TRUE(htable_lookup(ht_, &query.ht_entry) != NULL);
  strcpy(buffer, "Apple");
  ASSERT_TRUE(htable_lookup(ht_, &query.ht_entry) != NULL);
  strcpy(buffer, "Haruhi");
  ASSERT_TRUE(htable_lookup(ht_, &query.ht_entry) != NULL);
  strcpy(buffer, "Jacob");
  ASSERT_TRUE(htable_lookup(ht_, &query.ht_entry) != NULL);
  strcpy(buffer, "Orange");
  ASSERT_TRUE(htable_lookup(ht_, &query.ht_entry) == NULL);

  strcpy(buffer, "Banana");
  ht_str_entry *kv =
      ht_entry(htable_lookup(ht_, &query.ht_entry), ht_str_entry, ht_entry);
  EXPECT_EQ(kv->val, 25);

  strcpy(buffer, "Apple");
  kv = ht_entry(htable_lookup(ht_, &query.ht_entry), ht_str_entry, ht_entry);
  EXPECT_EQ(kv->val, 31);

  strcpy(buffer, "Haruhi");
  kv = ht_entry(htable_lookup(ht_, &query.ht_entry), ht_str_entry, ht_entry);
  EXPECT_EQ(kv->val, 30);

  strcpy(buffer, "Jacob");
  kv = ht_entry(htable_lookup(ht_, &query.ht_entry), ht_str_entry, ht_entry);
  EXPECT_EQ(kv->val, 9);

  strcpy(buffer, "Jacob");
  EXPECT_TRUE(htable_remove(ht_, &query.ht_entry));

  EXPECT_TRUE(htable_lookup(ht_, &query.ht_entry) == NULL);

  strcpy(buffer, "Banana");
  EXPECT_TRUE(htable_lookup(ht_, &query.ht_entry) != NULL);

  strcpy(buffer, "Moscow");
  EXPECT_FALSE(htable_remove(ht_, &query.ht_entry));

  strcpy(buffer, "Banana");
  EXPECT_TRUE(htable_remove(ht_, &query.ht_entry));
  EXPECT_TRUE(htable_lookup(ht_, &query.ht_entry) == NULL);

  EXPECT_EQ(htable_size(ht_), 2);

  strcpy(buffer, "Apple");
  kv = ht_entry(htable_lookup(ht_, &query.ht_entry), ht_str_entry, ht_entry);
  EXPECT_EQ(kv->val, 31);

  strcpy(buffer, "Haruhi");
  kv = ht_entry(htable_lookup(ht_, &query.ht_entry), ht_str_entry, ht_entry);
  EXPECT_EQ(kv->val, 30);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
