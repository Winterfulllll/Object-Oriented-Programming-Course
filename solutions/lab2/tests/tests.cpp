#include "../src/solution.h"
#include "gtest/gtest.h"
#include <iostream>

TEST(FiveBehavior, Addition) {
  Five f1("402");
  Five f2("1");
  Five f3 = f1 + f2;
  ASSERT_EQ(f3, Five("403"));
}

TEST(FiveBehavior, Subtraction) {
  Five f1("402");
  Five f2("401");
  Five f3 = f1 - f2;
  ASSERT_EQ(f3, Five("1"));
}

TEST(FiveBehavior, InvalidInitListException) {
  ASSERT_THROW(Five f1({'1', '2', '9'}), std::invalid_argument);
}

TEST(FiveBehavior, InvalidStringException) {
  ASSERT_THROW(Five f1("999"), std::invalid_argument);
}

TEST(FiveBehavior, SubtractionException) {
  Five f1("123");
  Five f2("100000");
  ASSERT_THROW(f1 -= f2, std::invalid_argument);
}

TEST(FiveBehavior, Comparison) {
  Five f1("123");
  Five f2("402");
  Five f3("123");
  ASSERT_TRUE(f1 < f2);
  ASSERT_TRUE(f2 > f1);
  ASSERT_TRUE(f1 <= f3);
  ASSERT_TRUE(f1 >= f3);
  ASSERT_TRUE(f1 == f3);
  ASSERT_TRUE(f1 != f2);
}

TEST(FiveBehavior, Output) {
  Five f("123");
  std::stringstream ss;
  ss << f;
  ASSERT_EQ(ss.str(), "123");
}

TEST(FiveBehavior, Zero) {
  Five f("0");
  ASSERT_EQ(f + f, f);
  ASSERT_EQ(f - f, f);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
