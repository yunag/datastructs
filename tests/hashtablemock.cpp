#include "datastructs/hash_table.h"
#include "gtest/gtest.h"
#include <gmock/gmock-function-mocker.h>
#include <memory>

using testing::_;

struct KeyValue {
  KeyValue() {}
  KeyValue(int key_, int val_ = 0) : key(key_), val(val_) {}

  int key;
  int val;
  hash_entry hh;
};

class IHashTable {
public:
  virtual ~IHashTable() = default;

  virtual size_t hash(const struct hash_entry *a) = 0;
  virtual bool equal(const struct hash_entry *a,
                     const struct hash_entry *b) = 0;
};

class HashTableMock : public IHashTable {
public:
  virtual ~HashTableMock() = default;

  MOCK_METHOD(size_t, hash, (const struct hash_entry *));
  MOCK_METHOD(bool, equal,
              (const struct hash_entry *, const struct hash_entry *));
};

class HashTableFixture : public ::testing::Test {
public:
  void SetUp() override { mock_ = new HashTableMock; }
  void TearDown() override { delete mock_; }

  static size_t hash(const struct hash_entry *a) { return mock_->hash(a); }

  static bool equal(const struct hash_entry *a, const struct hash_entry *b) {
    return mock_->equal(a, b);
  }

  static HashTableMock *mock_;
};

HashTableMock *HashTableFixture::mock_;

TEST_F(HashTableFixture, MyTest) {
  hash_table *ht_ =
      htable_create(1, HashTableFixture::hash, HashTableFixture::equal);
  KeyValue keyValue(0);

  EXPECT_CALL(*mock_, hash(&keyValue.hh)).Times(1);

  htable_insert(ht_, &keyValue.hh);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
