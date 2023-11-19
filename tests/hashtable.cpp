#include "gtest/gtest.h"

#include "datastructs/functions.h"
#include "datastructs/hash_table.h"

#include "helper.h"

#include <cassert>
#include <climits>
#include <cstring>
#include <unordered_map>
#include <utility>

typedef void (*destroy_entries)(hash_table *ht);

class HashTableTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override { htable_destroy(ht_, destroy_entries_); }

  void SetHashTable(hash_entry_fun hash, equal_ht_fun equal,
                    destroy_entries destroy, size_t size = 1) {
    destroy_entries_ = destroy;
    ht_ = htable_create(size, hash, equal);
    ASSERT_NE(ht_, nullptr);
  }

  destroy_entries destroy_entries_ = nullptr;
  hash_table *ht_ = nullptr;
};

struct ht_key_value {
  int key;
  int val;
  hash_entry he;
};

uint64_t hash_ht_key_value(const hash_entry *a) {
  ht_key_value *kva = ht_entry(a, ht_key_value, he);
  return yu_hash_i32(kva->key);
}

bool equal_ht_key_value(const hash_entry *a, const hash_entry *b) {
  ht_key_value *kva = ht_entry(a, ht_key_value, he);
  ht_key_value *kvb = ht_entry(b, ht_key_value, he);
  return kva->key == kvb->key;
}

int cmp_ht_key_value(const hash_entry *a, const hash_entry *b) {
  ht_key_value *kva = ht_entry(a, ht_key_value, he);
  ht_key_value *kvb = ht_entry(b, ht_key_value, he);
  if (kva->key > kvb->key) {
    return 1;
  }
  if (kva->key < kvb->key) {
    return -1;
  }
  return 0;
}

void destroy_kv_table(hash_table *ht) {
  ht_key_value *cur, *n;
  HTABLE_FOR_EACH_TEMP(ht, cur, n, he) { delete cur; }
}

ht_key_value *find_kv(hash_table *htable, int key) {
  ht_key_value kv = {.key = key};
  struct hash_entry *entry = htable_lookup(htable, &kv.he);

  return ht_entry_safe(entry, ht_key_value, he);
}

bool insert_kv(hash_table *htable, int key, int val) {
  ht_key_value *kv = find_kv(htable, key);
  if (kv) {
    kv->val = val;
    return true;
  }

  kv = new ht_key_value;
  kv->key = key;
  kv->val = val;

  if (!htable_insert(htable, &kv->he)) {
    delete kv;
    return false;
  }

  return true;
}

bool remove_kv(hash_table *htable, int key) {
  ht_key_value kv = {.key = key};
  struct hash_entry *entry = htable_remove(htable, &kv.he);

  if (entry) {
    delete ht_entry(entry, ht_key_value, he);
    return true;
  }

  return false;
}

struct ht_str_entry {
  char key[96];
  int val;
  struct hash_entry he;
};

uint64_t hash_ht_str_entry(const hash_entry *a) {
  ht_str_entry *kva = ht_entry(a, ht_str_entry, he);
  return yu_hash_str(kva->key);
}

bool equal_ht_str_entry(const hash_entry *a, const hash_entry *b) {
  ht_str_entry *kva = ht_entry(a, ht_str_entry, he);
  ht_str_entry *kvb = ht_entry(b, ht_str_entry, he);
  return strcmp(kva->key, kvb->key) == 0;
}

void destroy_str_table(hash_table *ht) {
  ht_str_entry *cur, *n;
  HTABLE_FOR_EACH_TEMP(ht, cur, n, he) { delete cur; }
}

bool insert_str(hash_table *htable, const char *str, int val) {
  struct hash_entry *replaced;

  ht_str_entry *kv = new ht_str_entry;
  assert(strlen(str) < sizeof(kv->key));

  strcpy(kv->key, str);
  kv->val = val;

  bool res = htable_replace(htable, &kv->he, &replaced);
  if (!res) {
    delete kv;
  }

  if (replaced) {
    delete ht_entry(replaced, ht_str_entry, he);
  }
  return res;
}

bool remove_str(hash_table *htable, const char *str) {
  ht_str_entry kv;
  assert(strlen(str) < sizeof(kv.key));

  strcpy(kv.key, str);
  struct hash_entry *entry = htable_remove(htable, &kv.he);

  if (entry) {
    delete ht_entry(entry, ht_str_entry, he);
    return true;
  }

  return false;
}

ht_str_entry *find_str(hash_table *htable, const char *str) {
  ht_str_entry kv;
  assert(strlen(str) < sizeof(kv.key));

  strcpy(kv.key, str);
  struct hash_entry *entry = htable_lookup(htable, &kv.he);

  return ht_entry_safe(entry, ht_str_entry, he);
}

TEST_F(HashTableTest, RemoveNotExistent) {
  SetHashTable(hash_ht_key_value, equal_ht_key_value, destroy_kv_table);

  remove_kv(ht_, 5);

  insert_kv(ht_, 5, 0);
  remove_kv(ht_, 6);

  remove_kv(ht_, 5);
  remove_kv(ht_, 5);
}

TEST_F(HashTableTest, STLTable) {
  SetHashTable(hash_ht_key_value, equal_ht_key_value, destroy_kv_table);

  enum class Action {
    Insert,
    Remove,
    Lookup,
  } command;

  std::unordered_map<int, int> stl;
  std::vector<int> keys;
  const size_t num_commands = 1000000;

  for (size_t i = 0; i < num_commands; ++i) {
    command = static_cast<Action>(Helper::rand_inrange(
        static_cast<int>(Action::Insert), static_cast<int>(Action::Lookup)));
    EXPECT_EQ(stl.size(), htable_size(ht_));

    if (stl.empty()) {
      command = Action::Insert;
    }

    switch (command) {
    case Action::Insert: {
      int key = Helper::rand_inrange(-10000, 10000);

      ht_key_value *found = find_kv(ht_, key);
      if (stl.find(key) != stl.end()) {
        ASSERT_NE(found, nullptr);
        EXPECT_EQ(stl.at(key), found->val);
      } else {
        ASSERT_EQ(found, nullptr);
        keys.push_back(key);
      }

      int val = Helper::rand_inrange(INT_MIN, INT_MAX);
      insert_kv(ht_, key, val);

      stl[key] = val;

      found = find_kv(ht_, key);
      ASSERT_NE(found, nullptr);

      EXPECT_EQ(stl.at(key), found->val);
      break;
    }

    case Action::Remove: {
      int idx = Helper::rand_inrange(0, keys.size() - 1);
      int key = keys[idx];

      ASSERT_NE(stl.find(key), stl.end());

      ht_key_value *found = find_kv(ht_, key);
      ASSERT_NE(found, nullptr);

      EXPECT_EQ(stl.at(key), found->val);

      stl.erase(key);
      remove_kv(ht_, key);
      keys.erase(keys.begin() + idx);

      EXPECT_EQ(stl.find(key), stl.end());
      EXPECT_EQ(find_kv(ht_, key), nullptr);
      break;
    }

    case Action::Lookup: {
      int idx = Helper::rand_inrange(0, keys.size() - 1);
      int key = keys[idx];

      ht_key_value *found = find_kv(ht_, key);

      if (stl.find(key) != stl.end()) {
        ASSERT_NE(found, nullptr);
        EXPECT_EQ(stl.at(key), found->val);
      } else {
        ASSERT_EQ(found, nullptr);
      }
      break;
    }
    }
  }
}

TEST_F(HashTableTest, Case1) {
  SetHashTable(hash_ht_key_value, equal_ht_key_value, destroy_kv_table);
  std::unordered_map<int, int> stl_map;
  std::vector<std::pair<int, int>> kvalues = {
      {8, 9},
      {8, 10},
      {9, 25},
      {9, 30},
  };

  int cycles;
  for (const auto &kv : kvalues) {
    insert_kv(ht_, kv.first, kv.second);
    stl_map[kv.first] = kv.second;
  }

  ht_key_value *kv;

  cycles = 0;
  HTABLE_FOR_EACH(ht_, kv, he) {
    cycles++;
    ASSERT_EQ(kv->val, stl_map[kv->key]);
  }
  ASSERT_EQ(cycles, 2);

  cycles = 0;
  HTABLE_FOR_EACH(ht_, kv, he) {
    cycles++;
    ASSERT_EQ(kv->val, stl_map[kv->key]);
    if (cycles == 1) {
      break;
    }
  }
  ASSERT_EQ(cycles, 1);
}

TEST_F(HashTableTest, SortTable) {
  SetHashTable(hash_ht_key_value, equal_ht_key_value, destroy_kv_table);
  std::vector<int> values = {5, 7, 1,   8, 2, 227, 80, 117, 2000, -5, -7, 9,
                             3, 5, 100, 8, 9, 10,  22, 2,   1,    5,  7,  9};
  for (int val : values) {
    insert_kv(ht_, val, 0);
  }

  /* Sort ascending */
  htable_sort(ht_, cmp_ht_key_value);

  struct ht_key_value *kv;
  HTABLE_FOR_EACH(ht_, kv, he) { kv->key = 5; }

  size_t num_iters = 0;
  int val = INT_MIN;

  HTABLE_FOR_EACH(ht_, kv, he) {
    ASSERT_GE(kv->key, val);

    num_iters++;
    val = kv->key;
  }
  ASSERT_EQ(htable_size(ht_), num_iters);
}

TEST_F(HashTableTest, Strings) {
  SetHashTable(hash_ht_str_entry, equal_ht_str_entry, destroy_str_table);

  std::vector<std::pair<const char *, int>> kvalues = {
      {"Jacob", 9},   {"Banana", 10}, {"Banana", 25},
      {"Haruhi", 30}, {"Apple", 31},
  };

  for (const auto &kv : kvalues) {
    insert_str(ht_, kv.first, kv.second);
  }

  ASSERT_NE(find_str(ht_, "Banana"), nullptr);
  ASSERT_NE(find_str(ht_, "Apple"), nullptr);
  ASSERT_NE(find_str(ht_, "Haruhi"), nullptr);
  ASSERT_NE(find_str(ht_, "Jacob"), nullptr);
  ASSERT_EQ(find_str(ht_, "Orange"), nullptr);

  ht_str_entry *kv = find_str(ht_, "Banana");
  EXPECT_EQ(kv->val, 25);

  kv = find_str(ht_, "Apple");
  EXPECT_EQ(kv->val, 31);

  kv = find_str(ht_, "Haruhi");
  EXPECT_EQ(kv->val, 30);

  kv = find_str(ht_, "Jacob");
  EXPECT_EQ(kv->val, 9);

  EXPECT_TRUE(remove_str(ht_, "Jacob"));
  ASSERT_EQ(find_str(ht_, "Jacob"), nullptr);

  EXPECT_NE(find_str(ht_, "Banana"), nullptr);

  EXPECT_FALSE(remove_str(ht_, "Africa"));

  EXPECT_TRUE(remove_str(ht_, "Banana"));
  EXPECT_EQ(find_str(ht_, "Banana"), nullptr);

  EXPECT_EQ(htable_size(ht_), 2);

  kv = find_str(ht_, "Apple");
  EXPECT_EQ(kv->val, 31);

  kv = find_str(ht_, "Haruhi");
  EXPECT_EQ(kv->val, 30);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
