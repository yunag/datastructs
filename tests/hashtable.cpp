#include "gtest/gtest.h"

#include "datastructs/functions.h"
#include "datastructs/hash_table.h"

#include "utils.hpp"

struct KeyValue {
  KeyValue() {}
  KeyValue(int key_, int val_ = 0) : key(key_), val(val_) {}

  int key;
  int val;
  hash_entry hh;
};

bool equalKeyValue(const hash_entry *a, const hash_entry *b) {
  KeyValue *firstKeyValue = ht_entry(a, KeyValue, hh);
  KeyValue *secondKeyValue = ht_entry(b, KeyValue, hh);

  return firstKeyValue->key == secondKeyValue->key;
}

size_t hashKeyValueNode(const hash_entry *a) {
  KeyValue *keyValue = ht_entry(a, KeyValue, hh);
  return yu_hash_i32(keyValue->key);
}

class HashTable {
public:
  HashTable() { ht_ = htable_create(1, hashKeyValueNode, equalKeyValue); }

  ~HashTable() {
    KeyValue *cur, *n;

    ht_for_each_temp(ht_, cur, n, hh) { delete cur; }
  }

  void insert(int key, int val = 0) {
    KeyValue query(key);

    KeyValue *found = find(key);
    if (found) {
      found->val = val;
      return;
    }

    KeyValue *keyValue = new KeyValue(key, val);
    htable_insert(ht_, &keyValue->hh);
  }

  KeyValue *find(int key) {
    KeyValue query(key);

    return ht_find(ht_, &query, hh);
  }

  void remove(int key) {
    KeyValue query(key);

    KeyValue *removeKeyValue = ht_remove(ht_, &query, hh);

    delete removeKeyValue;
  }

  hash_entry *first() { return htable_first(ht_); }
  hash_entry *last() { return htable_last(ht_); }

  size_t size() { return htable_size(ht_); }
  hash_table *container() { return ht_; }

private:
  hash_table *ht_;
};

class HashTableTestFixture : public ::testing::Test {
protected:
  void SetUp() override {
    for (int i = 0; i < 10; ++i) {
      ht_.insert(i, i);
    }
  }

  void TearDown() override {}

  HashTable ht_;
};

TEST(HashTableTest, AVLInit_DefaultInitialization_ReturnsInitializedAVL) {
  HashTable hashTable;

  size_t hashTableSize = hashTable.size();
  hash_entry *firstEntry = hashTable.first();
  hash_entry *lastEntry = hashTable.last();

  EXPECT_EQ(hashTableSize, 0);
  EXPECT_FALSE(notNull(firstEntry));
  EXPECT_FALSE(notNull(lastEntry));
}

TEST(HashTableTest, Size_InsertDistinctItems_ReturnsValidSize) {
  HashTable hashTable;

  const int numItems = 10;
  for (int i = 0; i < numItems; ++i) {
    hashTable.insert(i, i);
  }

  size_t hashTableSize = hashTable.size();

  ASSERT_EQ(hashTableSize, numItems);
}

TEST(HashTableTest, Size_InsertSameItemMultipleTimes_ReturnsOne) {
  HashTable hashTable;

  hashTable.insert(0);
  hashTable.insert(0);

  size_t hashTableSize = hashTable.size();

  ASSERT_EQ(hashTableSize, 1);
}

TEST(HashTableTest, Size_InsertRemoveInsert_ReturnsOne) {
  HashTable hashTable;

  hashTable.insert(0);
  hashTable.remove(0);
  hashTable.insert(0);

  size_t hashTableSize = hashTable.size();

  ASSERT_EQ(hashTableSize, 1);
}

TEST_F(HashTableTestFixture, Size_RemoveSingleItem_ReturnsValidSize) {
  size_t hashTableSizeBefore = ht_.size();

  ht_.remove(0);

  size_t hashTableSizeAfter = ht_.size();

  ASSERT_EQ(hashTableSizeBefore, hashTableSizeAfter + 1);
}

TEST_F(HashTableTestFixture, Find_FindExistingItem_ReturnsItem) {
  KeyValue *found = ht_.find(0);

  int foundKey = found->key;
  int foundVal = found->val;

  ASSERT_TRUE(notNull(found));

  EXPECT_EQ(foundKey, 0);
  EXPECT_EQ(foundVal, 0);
}

TEST_F(HashTableTestFixture, Find_FindNonExistingItem_ReturnsNull) {
  KeyValue *found = ht_.find(-1);

  ASSERT_FALSE(notNull(found));
}

TEST_F(HashTableTestFixture, Find_RemoveExistingItem_ReturnsNull) {
  ht_.remove(0);

  KeyValue *found = ht_.find(0);

  ASSERT_FALSE(notNull(found));
}

TEST_F(HashTableTestFixture, Find_RemoveNonExistingItem_ReturnsNull) {
  ht_.remove(-1);

  KeyValue *found = ht_.find(-1);

  ASSERT_FALSE(notNull(found));
}

TEST_F(HashTableTestFixture, ForEach_Default_ReturnsValidNumberOfIterations) {
  KeyValue *keyValue;
  hash_table *hashTable = ht_.container();

  size_t numIters = 0;

  ht_for_each(hashTable, keyValue, hh) { numIters++; }

  size_t hashTableSize = ht_.size();

  ASSERT_EQ(hashTableSize, numIters);
}

TEST_F(HashTableTestFixture,
       ForEach_RemoveMultipleItems_ReturnsValidNumberOfIterations) {
  ht_.remove(0);
  ht_.remove(1);
  ht_.remove(2);

  KeyValue *keyValue;
  hash_table *hashTable = ht_.container();

  size_t numIters = 0;

  ht_for_each(hashTable, keyValue, hh) { numIters++; }

  size_t hashTableSize = ht_.size();

  ASSERT_EQ(hashTableSize, numIters);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
