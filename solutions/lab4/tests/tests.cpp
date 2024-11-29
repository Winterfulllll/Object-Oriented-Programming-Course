#include "../src/solution.h"
#include "gtest/gtest.h"

TEST(PointTest, Constructor) {
  Point<int> p1(1, 2);
  ASSERT_EQ(p1.get_x(), 1);
  ASSERT_EQ(p1.get_y(), 2);

  Point<double> p2(3.14, 2.71);
  ASSERT_NEAR(p2.get_x(), 3.14, 0.001);
  ASSERT_NEAR(p2.get_y(), 2.71, 0.001);
}

TEST(PointTest, Equality) {
  Point<int> p1(1, 2);
  Point<int> p2(1, 2);
  Point<int> p3(3, 4);
  ASSERT_TRUE(p1 == p2);
  ASSERT_FALSE(p1 == p3);
}

TEST(FigureTest, Constructor) {
  Point<int> p1(0, 0), p2(1, 0), p3(0, 1);
  Figure<int> f({p1, p2, p3});
  ASSERT_EQ(f.get_size(), 3);

  ASSERT_THROW(Figure<int>({p1, p2}), std::runtime_error);
}

TEST(FigureTest, Barycenter) {
  Point<double> p1(0, 0), p2(2, 0), p3(0, 2);
  Figure<double> f({p1, p2, p3});
  Point<double> barycenter = f.get_barycenter();
  ASSERT_NEAR(barycenter.get_x(), 2.0 / 3.0, 0.001);
  ASSERT_NEAR(barycenter.get_y(), 2.0 / 3.0, 0.001);
}

TEST(FigureTest, Area) {
  Point<double> p1(0, 0), p2(2, 0), p3(0, 2);
  Figure<double> f({p1, p2, p3});
  double area = static_cast<double>(f);
  ASSERT_NEAR(area, 2.0, 0.001);
}

TEST(SquareTest, ConstructorSide) {
  Point<int> start(1, 2);
  Square<int> s(start, 5);
  ASSERT_EQ(s.get_size(), 4);
  ASSERT_EQ(s.get_side(), 5);

  ASSERT_THROW(Square<int>(start, -5), std::invalid_argument);
}

TEST(SquareTest, ConstructorList) {
  Point<int> p1(0, 0), p2(5, 0), p3(5, 5), p4(0, 5);
  Square<int> s({p1, p2, p3, p4});
  ASSERT_EQ(s.get_size(), 4);
  ASSERT_EQ(s.get_side(), 5);

  ASSERT_THROW(Square<int>({p1, p2, p3}), std::invalid_argument);
  ASSERT_THROW(Square<int>({p1, Point<int>(10, 0), p3, p4}),
               std::invalid_argument);
}

TEST(SquareTest, Area) {
  Point<double> start(1, 2);
  Square<double> s(start, 5);
  double area = static_cast<double>(s);
  ASSERT_NEAR(area, 25.0, 0.001);
}

// ... (Аналогичные тесты для Rectangle и Trapezoid)

TEST(TrapezoidTest, ConstructorPoints) {
  Point<double> p1(0, 0), p2(4, 0), p3(3, 2), p4(1, 2);
  Trapezoid<double> t(p1, p2, p3, p4);
  ASSERT_NEAR(static_cast<double>(t), 6, 0.001);
}

TEST(TrapezoidTest, ConstructorList) {
  Point<double> p1(0, 0), p2(4, 0), p3(3, 2), p4(1, 2);
  Trapezoid<double> t({p1, p2, p3, p4});
  ASSERT_NEAR(static_cast<double>(t), 6, 0.001);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
