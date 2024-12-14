#include "../src/solution.h"
#include "gtest/gtest.h"
#include <sstream>

class SquareTest : public ::testing::Test {
protected:
  void SetUp() override { square = new Square(Point(0, 0), 5.0); }

  void TearDown() override { delete square; }

  Square *square;
};

TEST_F(SquareTest, Initialization) {
  Point p1(0, 0);
  Square sq(p1, 4.0);
  ASSERT_EQ(static_cast<double>(sq), 16.0);
}

TEST_F(SquareTest, Barycenter) {
  Point p1(0, 0);
  Square sq(p1, 4.0);
  Point barycenter = sq.get_barycenter();
  ASSERT_NEAR(barycenter.getX(), 2.0, 0.001);
  ASSERT_NEAR(barycenter.getY(), 2.0, 0.001);
}

class RectangleTest : public ::testing::Test {
protected:
  void SetUp() override { rectangle = new Rectangle(Point(0, 0), 4.0, 8.0); }

  void TearDown() override { delete rectangle; }

  Rectangle *rectangle;
};

TEST_F(RectangleTest, Initialization) {
  Point p1(0, 0);
  Rectangle rect(p1, 4.0, 6.0);
  ASSERT_EQ(static_cast<double>(rect), 24.0);
}

TEST_F(RectangleTest, Barycenter) {
  Point p1(0, 0);
  Rectangle rect(p1, 4.0, 6.0);
  Point barycenter = rect.get_barycenter();
  ASSERT_NEAR(barycenter.getX(), 2.0, 0.001);
  ASSERT_NEAR(barycenter.getY(), 3.0, 0.001);
}

class TrapezoidTest : public ::testing::Test {
protected:
  void SetUp() override {
    trapezoid =
        new Trapezoid({Point(0, 0), Point(10, 0), Point(8, 10), Point(2, 10)});
  }

  void TearDown() override { delete trapezoid; }

  Trapezoid *trapezoid;
};

TEST_F(TrapezoidTest, Initialization) {
  Trapezoid tr({Point(0, 0), Point(10, 0), Point(8, 10), Point(2, 10)});
  ASSERT_NEAR(static_cast<double>(tr), 80.0, 0.001);
}

TEST_F(TrapezoidTest, Barycenter) {
  Point barycenter = trapezoid->get_barycenter();
  ASSERT_NEAR(barycenter.getX(), 5.0, 0.001);
  ASSERT_NEAR(barycenter.getY(), 5.0, 0.001);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
