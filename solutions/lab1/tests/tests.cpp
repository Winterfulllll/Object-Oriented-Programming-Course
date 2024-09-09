#include <string>
#include "gtest/gtest.h"
#include "../src/solution.h"

TEST(RemoveVowelsTest, Test1) {
   std::string input = "This website is for losers LOL!";
   std::string expected = "Ths wbst s fr lsrs LL!";
   EXPECT_EQ(RemoveVowels(input), expected);
}

TEST(RemoveVowelsTest, Test2) {
   std::string input = "ABCabc?!";
   std::string expected = "BCbc?!";
   EXPECT_EQ(RemoveVowels(input), expected);
}

int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
