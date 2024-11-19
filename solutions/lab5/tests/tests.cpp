#include "../src/solution.cpp"
#include "gtest/gtest.h"

TEST(DynamicMemoryResourceTest, AllocateAndDeallocate) {
  DynamicMemoryResource mr;
  void *p1 = mr.allocate(100, alignof(std::max_align_t));
  ASSERT_NE(nullptr, p1);
  mr.deallocate(p1, 100, alignof(std::max_align_t));

  void *p2 = mr.allocate(200, alignof(std::max_align_t));
  ASSERT_NE(nullptr, p2);
  ASSERT_NE(p1, p2);

  mr.deallocate(p2, 200, alignof(std::max_align_t));
}

TEST(DynamicMemoryResourceTest, DeallocateInvalidPointer) {
  DynamicMemoryResource mr;
  void *p = reinterpret_cast<void *>(0xDEADBEEF);
  mr.deallocate(p, 100, alignof(std::max_align_t));
}

TEST(DynamicMemoryResourceTest, DeallocateWrongSize) {
  DynamicMemoryResource mr;
  void *p = mr.allocate(100, alignof(std::max_align_t));
  ASSERT_NE(nullptr, p);
  ASSERT_DEATH(mr.deallocate(p, 200, alignof(std::max_align_t)), "");

  mr.deallocate(p, 100, alignof(std::max_align_t));
}

TEST(DynamicArrayTest, BasicOperations) {
  DynamicMemoryResource mr;
  DynamicArray<int> arr{&mr};

  arr.push_back(1);
  arr.push_back(2);
  arr.push_back(3);

  ASSERT_EQ(3, arr.size());
  ASSERT_EQ(1, arr[0]);
  ASSERT_EQ(2, arr[1]);
  ASSERT_EQ(3, arr[2]);

  arr.clear();
  ASSERT_EQ(0, arr.size());
}

TEST(DynamicArrayTest, Reserve) {
  DynamicMemoryResource mr;
  DynamicArray<int> arr{&mr};

  arr.reserve(5);
  ASSERT_EQ(0, arr.size());

  arr.push_back(1);
  arr.push_back(2);

  ASSERT_EQ(2, arr.size());
}

TEST(DynamicArrayTest, CopyConstructor) {
  DynamicMemoryResource mr;
  DynamicArray<int> arr1{&mr};
  arr1.push_back(1);
  arr1.push_back(2);

  DynamicArray<int> arr2 = arr1;

  ASSERT_EQ(2, arr2.size());
  ASSERT_EQ(1, arr2[0]);
  ASSERT_EQ(2, arr2[1]);

  arr1[0] = 3;
  ASSERT_EQ(1, arr2[0]);
}

TEST(DynamicArrayTest, MoveConstructor) {
  DynamicMemoryResource mr;
  DynamicArray<int> arr1{&mr};
  arr1.push_back(1);
  arr1.push_back(2);

  DynamicArray<int> arr2 = std::move(arr1);

  ASSERT_EQ(2, arr2.size());
  ASSERT_EQ(1, arr2[0]);
  ASSERT_EQ(2, arr2[1]);
  ASSERT_EQ(0, arr1.size());
  ASSERT_EQ(nullptr, arr1.begin().get_ptr());
}

TEST(DynamicArrayTest, Iterator) {
  DynamicMemoryResource mr;
  DynamicArray<int> arr{&mr};

  arr.push_back(1);
  arr.push_back(2);
  arr.push_back(3);

  int count = 1;
  for (int x : arr) {
    ASSERT_EQ(count, x);
    ++count;
  }
}

struct MyStruct {
  int a;
  double b;
  std::string c;

  bool operator==(const MyStruct &other) const {
    return a == other.a && b == other.b && c == other.c;
  }
};

TEST(DynamicArrayTest, ComplexType) {
  DynamicMemoryResource mr;
  DynamicArray<MyStruct> arr{&mr};

  arr.push_back({1, 2.5, "hello"});
  arr.push_back({2, 3.14, "world"});

  ASSERT_EQ(2, arr.size());
  ASSERT_EQ((MyStruct{1, 2.5, "hello"}), arr[0]);
  ASSERT_EQ((MyStruct{2, 3.14, "world"}), arr[1]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
