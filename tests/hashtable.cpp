#include "gtest/gtest.h"

#include <vector>
#include <climits>

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
  KeyValue *firstKeyValue = htable_entry(a, KeyValue, hh);
  KeyValue *secondKeyValue = htable_entry(b, KeyValue, hh);

  return firstKeyValue->key == secondKeyValue->key;
}

bool lessKeyValue(const hash_entry *a, const hash_entry *b) {
  KeyValue *firstKeyValue = htable_entry(a, KeyValue, hh);
  KeyValue *secondKeyValue = htable_entry(b, KeyValue, hh);

  return firstKeyValue->key < secondKeyValue->key;
}

size_t hashKeyValueNode(const hash_entry *a) {
  KeyValue *keyValue = htable_entry(a, KeyValue, hh);
  return yu_hash_i32(keyValue->key);
}

class HashTable {
public:
  HashTable() { ht_ = htable_create(1, hashKeyValueNode, equalKeyValue); }

  ~HashTable() {
    KeyValue *cur, *n;

    htable_for_each_temp(ht_, cur, n, hh) { delete cur; }
    htable_destroy(ht_, nullptr);
  }

  void insert(int key, int val = 0) {
    KeyValue query(key);

    KeyValue *found = find(key);
    if (found) {
      found->val = val;
      return;
    }

    KeyValue *keyValue = new KeyValue(key, val);

    bool isInserted = htable_add(ht_, keyValue, hh);
    ASSERT_TRUE(isInserted);
  }

  void replace(int key, int val = 0) {
    KeyValue *keyValue = new KeyValue(key, val);
    hash_entry *replaced;

    htable_replace(ht_, &keyValue->hh, &replaced);
    if (replaced) {
      delete htable_entry(replaced, KeyValue, hh);
    }
  }

  bool rehash(size_t new_num_buckets) {
    return htable_rehash(ht_, new_num_buckets);
  }

  KeyValue *find(int key) {
    KeyValue query(key);

    return htable_find(ht_, &query, hh);
  }

  void remove(int key) {
    KeyValue query(key);

    KeyValue *removeKeyValue = htable_delete(ht_, &query, hh);

    delete removeKeyValue;
  }

  void sort() { htable_sort(ht_, lessKeyValue); }

  hash_entry *first() { return htable_first(ht_); }
  hash_entry *last() { return htable_last(ht_); }

  size_t size() { return htable_size(ht_); }
  size_t num_buckets() { return htable_num_buckets(ht_); }

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

TEST(HashTableTest, Rehash_InsertItemTriggersRehash_ReturnsGreaterNumBuckets) {
  HashTable hashTable;

  size_t numBucketsBefore = hashTable.num_buckets();
  hashTable.insert(0);
  hashTable.insert(1);
  size_t numBucketsAfter = hashTable.num_buckets();

  ASSERT_GT(numBucketsAfter, numBucketsBefore);
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

TEST(HashTableTest, Insert_InsertAndFind_ReturnsEntry) {
  HashTable hashTable;

  hashTable.insert(0);

  KeyValue *found = hashTable.find(0);

  ASSERT_TRUE(notNull(found));
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

TEST_F(HashTableTestFixture, Rehash_Default_ReturnsNewNumberOfBuckets) {
  size_t numBucketsWeWant = 200;
  ht_.rehash(numBucketsWeWant);

  size_t numBucketsWeGot = ht_.num_buckets();

  ASSERT_EQ(numBucketsWeWant, numBucketsWeGot);
}

TEST_F(HashTableTestFixture, ForEach_Default_ReturnsValidNumberOfIterations) {
  KeyValue *keyValue;
  hash_table *hashTable = ht_.container();

  size_t numIters = 0;

  htable_for_each(hashTable, keyValue, hh) { numIters++; }

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

  htable_for_each(hashTable, keyValue, hh) { numIters++; }

  size_t hashTableSize = ht_.size();

  ASSERT_EQ(hashTableSize, numIters);
}

struct SortTestCase {
public:
  SortTestCase(std::vector<int> input_, std::vector<int> expected_)
      : input(std::move(input_)), expected(std::move(expected_)) {}

  void run(HashTable &hashTable) {
    populateTable(hashTable);
    hashTable.sort();
    compareWithExpected(hashTable);
  }

private:
  void populateTable(HashTable &hashTable) {
    for (int num : input) {
      hashTable.insert(num);
    }
  }

  void compareWithExpected(HashTable &hashTable) {
    hash_table *htable = hashTable.container();
    KeyValue *keyValue;

    std::vector<int> got;

    htable_for_each(htable, keyValue, hh) { got.push_back(keyValue->key); }

    EXPECT_EQ(expected, got);
  }

  std::vector<int> input;
  std::vector<int> expected;
};

TEST(HashTableTest, Sort_TestCases_ReturnsAscendingTable) {
  std::vector<SortTestCase> testcases = {
      SortTestCase({8, 7, 6, 5, 4, 3, 2, 1, 0}, {0, 1, 2, 3, 4, 5, 6, 7, 8}),
      SortTestCase({7, 5, 1, 0, 9, -5}, {-5, 0, 1, 5, 7, 9}),
      SortTestCase({0, 1, 2, 3, 4}, {0, 1, 2, 3, 4}),
      SortTestCase({7, 8, 9, 10, 4, 0, 1, 2}, {0, 1, 2, 4, 7, 8, 9, 10}),
      SortTestCase({-9, -10, -8, -6, -7, 5}, {-10, -9, -8, -7, -6, 5}),
      SortTestCase(
          {1, 0, 5, 4, 200, 555, 88, 300, 10, 27, -500, -27, INT_MAX, INT_MIN},
          {INT_MIN, -500, -27, 0, 1, 4, 5, 10, 27, 88, 200, 300, 555, INT_MAX}),
  };

  for (SortTestCase &testcase : testcases) {
    HashTable hashTable;

    testcase.run(hashTable);
  }
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
