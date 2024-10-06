#pragma once

#include <initializer_list>
#include <iostream>

class Point {
public:
  Point();
  Point(double x, double y);
  Point(const Point &other) = default;
  Point(Point &&other) noexcept = default;
  ~Point() = default;

  double getX() const;
  double getY() const;

  Point &operator=(const Point &other);
  Point &operator=(Point &&other) noexcept;
  bool operator==(const Point &other) const;

  friend std::ostream &operator<<(std::ostream &os, const Point &p);
  friend std::istream &operator>>(std::istream &is, Point &p);

private:
  double _x = 0.0;
  double _y = 0.0;
};

class Figure {
public:
  Figure();
  Figure(const std::initializer_list<Point> &lst);
  Figure(const Figure &other);
  Figure(Figure &&other) noexcept;
  virtual ~Figure() noexcept;

  Figure &operator=(const Figure &other);
  Figure &operator=(Figure &&other) noexcept;
  bool operator==(const Figure &other);

  Point get_barycenter() const;
  virtual explicit operator double() const;

  friend std::ostream &operator<<(std::ostream &os, const Figure &fig);
  friend std::istream &operator>>(std::istream &is, Figure &fig);

protected:
  size_t _points_count;
  Point *_points;
};

class Square : public Figure {
public:
  Square(Point start, double side);
  Square(const std::initializer_list<Point> &lst);

  Square(const Square &other);
  Square(Square &&other) noexcept;
  Square &operator=(const Square &other);
  Square &operator=(Square &&other) noexcept;

  operator double() const override;

private:
  double _side;
};

class Rectangle : public Figure {
public:
  Rectangle(Point start, double width, double height);
  Rectangle(const std::initializer_list<Point> &lst);

  Rectangle(const Rectangle &other);
  Rectangle(Rectangle &&other) noexcept;
  Rectangle &operator=(const Rectangle &other);
  Rectangle &operator=(Rectangle &&other) noexcept;

  operator double() const override;

private:
  double _width, _height;
};

class Trapezoid : public Figure {
public:
  Trapezoid(const std::initializer_list<Point> &lst);

  Trapezoid(const Trapezoid &other);
  Trapezoid(Trapezoid &&other) noexcept;
  Trapezoid &operator=(const Trapezoid &other);
  Trapezoid &operator=(Trapezoid &&other) noexcept;

  operator double() const override;

private:
  static bool is_collinear(const Point &v1, const Point &v2);
  static bool are_equal(const Point &v1, const Point &v2);
  static double distance(const Point &p1, const Point &p2);
  static double triangleArea(const Point &a, const Point &b, const Point &c);

private:
  double _base1, _base2, _height;
};
