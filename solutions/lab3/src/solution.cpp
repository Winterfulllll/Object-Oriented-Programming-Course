#include "solution.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <stdexcept>

// POINT REALIZATION

Point::Point() = default;
Point::Point(double x, double y) : _x(x), _y(y) {}

double Point::getX() const { return _x; }
double Point::getY() const { return _y; }

Point &Point::operator=(const Point &other) {
  if (this != &other) {
    _x = other._x;
    _y = other._y;
  }
  return *this;
}

Point &Point::operator=(Point &&other) noexcept {
  if (this != &other) {
    _x = other._x;
    _y = other._y;
  }
  return *this;
}

bool Point::operator==(const Point &other) const {
  return std::abs(_x - other._x) < std::numeric_limits<double>::epsilon() &&
         std::abs(_y - other._y) < std::numeric_limits<double>::epsilon();
}

std::ostream &operator<<(std::ostream &os, const Point &p) {
  os << "(" << p.getX() << ", " << p.getY() << ")";
  return os;
}

std::istream &operator>>(std::istream &is, Point &p) {
  double x, y;
  is >> x >> y;
  p = Point(x, y);
  return is;
}

// FIGURE REALIZATION

Figure::Figure() : _points_count(0), _points(nullptr) {}

Figure::Figure(const std::initializer_list<Point> &lst)
    : _points_count(lst.size()), _points(new Point[lst.size()]) {
  if (_points_count < 3) {
    throw std::runtime_error("Figure must have at least 3 points");
  }
  std::copy(lst.begin(), lst.end(), _points);
}

Figure::Figure(const Figure &other)
    : _points_count(other._points_count),
      _points(new Point[other._points_count]) {
  std::copy(other._points, other._points + other._points_count, _points);
}

Figure::Figure(Figure &&other) noexcept
    : _points_count(other._points_count), _points(other._points) {
  other._points_count = 0;
  other._points = nullptr;
}

Figure &Figure::operator=(const Figure &other) {
  if (this != &other) {
    Figure temp(other);
    std::swap(_points_count, temp._points_count);
    std::swap(_points, temp._points);
  }
  return *this;
}

Figure &Figure::operator=(Figure &&other) noexcept {
  if (this != &other) {
    delete[] _points;
    _points_count = other._points_count;
    _points = other._points;
    other._points_count = 0;
    other._points = nullptr;
  }
  return *this;
}

bool Figure::operator==(const Figure &other) {
  if (_points_count != other._points_count) {
    return false;
  }
  for (size_t i = 0; i < _points_count; ++i) {
    if (_points[i] != other._points[i]) {
      return false;
    }
  }
  return true;
}

Figure::~Figure() noexcept { delete[] _points; }

Point Figure::get_barycenter() const {
  double sum_x = 0;
  double sum_y = 0;

  for (size_t i = 0; i < _points_count; ++i) {
    sum_x += _points[i].getX();
    sum_y += _points[i].getY();
  }

  return Point(sum_x / _points_count, sum_y / _points_count);
}

std::ostream &operator<<(std::ostream &os, const Figure &fig) {
  os << "Figure with " << fig._points_count << " points: {";
  for (size_t i = 0; i < fig._points_count; ++i) {
    os << fig._points[i] << (i < fig._points_count - 1 ? ", " : "");
  }
  os << "}";
  return os;
}

std::istream &operator>>(std::istream &is, Figure &fig) {
  std::cout << "Enter the number of points: ";
  size_t points_count;
  is >> points_count;

  delete[] fig._points;
  fig._points = new Point[points_count];
  fig._points_count = points_count;

  for (size_t i = 0; i < fig._points_count; ++i) {
    std::cout << "Enter point " << i + 1 << " (x y): ";
    is >> fig._points[i];
  }
  return is;
}

Figure::operator double() const {
  double area = 0.0;
  for (size_t i = 0; i < _points_count; ++i) {
    size_t j = (i + 1) % _points_count;
    area += (_points[i].getX() * _points[j].getY() -
             _points[j].getX() * _points[i].getY());
  }
  return std::abs(area) / 2.0;
}

// SQUARE REALIZATION

Square::Square(const Square &other) : Figure(other), _side(other._side) {}

Square::Square(Square &&other) noexcept
    : Figure(std::move(other)), _side(other._side) {
  other._side = 0;
}

Square &Square::operator=(const Square &other) {
  if (this != &other) {
    Figure::operator=(other);
    _side = other._side;
  }
  return *this;
}

Square &Square::operator=(Square &&other) noexcept {
  if (this != &other) {
    Figure::operator=(std::move(other));
    _side = other._side;
    other._side = 0;
  }
  return *this;
}

Square::Square(Point start, double side) : _side(side) {
  if (side <= 0) {
    throw std::invalid_argument("Side length must be positive");
  }

  delete[] _points;
  _points_count = 4;
  _points = new Point[_points_count];

  _points[0] = start;
  _points[1] = Point(start.getX() + side, start.getY());
  _points[2] = Point(start.getX() + side, start.getY() + side);
  _points[3] = Point(start.getX(), start.getY() + side);
}

Square::Square(const std::initializer_list<Point> &lst) : Figure(lst) {
  if (lst.size() != 4) {
    throw std::invalid_argument("Square must have 4 points");
  }

  double side1 = std::sqrt(
      std::pow((*lst.begin()).getX() - (*(lst.begin() + 1)).getX(), 2) +
      std::pow((*lst.begin()).getY() - (*(lst.begin() + 1)).getY(), 2));
  double side2 = std::sqrt(
      std::pow((*(lst.begin() + 1)).getX() - (*(lst.begin() + 2)).getX(), 2) +
      std::pow((*(lst.begin() + 1)).getY() - (*(lst.begin() + 2)).getY(), 2));

  if (std::abs(side1 - side2) > std::numeric_limits<double>::epsilon()) {
    throw std::invalid_argument("The provided points do not form a square");
  }

  _side = side1;
}

Square::operator double() const { return _side * _side; }

// RECTANGLE REALIZATION

Rectangle::Rectangle(Point start, double width, double height)
    : _width(width), _height(height) {
  if (width <= 0 || height <= 0) {
    throw std::invalid_argument("Width and height must be positive");
  }

  delete[] _points;
  _points_count = 4;
  _points = new Point[_points_count];

  _points[0] = start;
  _points[1] = Point(start.getX() + width, start.getY());
  _points[2] = Point(start.getX() + width, start.getY() + height);
  _points[3] = Point(start.getX(), start.getY() + height);
}

Rectangle::Rectangle(const std::initializer_list<Point> &lst) : Figure(lst) {
  if (lst.size() != 4) {
    throw std::invalid_argument("Rectangle must have 4 points");
  }

  double width1 = std::sqrt(
      std::pow((*lst.begin()).getX() - (*(lst.begin() + 1)).getX(), 2) +
      std::pow((*lst.begin()).getY() - (*(lst.begin() + 1)).getY(), 2));
  double height1 = std::sqrt(
      std::pow((*(lst.begin() + 1)).getX() - (*(lst.begin() + 2)).getX(), 2) +
      std::pow((*(lst.begin() + 1)).getY() - (*(lst.begin() + 2)).getY(), 2));
  double width2 = std::sqrt(
      std::pow((*(lst.begin() + 2)).getX() - (*(lst.begin() + 3)).getX(), 2) +
      std::pow((*(lst.begin() + 2)).getY() - (*(lst.begin() + 3)).getY(), 2));
  double height2 = std::sqrt(
      std::pow((*(lst.begin() + 3)).getX() - (*(lst.begin())).getX(), 2) +
      std::pow((*(lst.begin() + 3)).getY() - (*(lst.begin())).getY(), 2));

  if (std::abs(width1 - width2) > std::numeric_limits<double>::epsilon() ||
      std::abs(height1 - height2) > std::numeric_limits<double>::epsilon()) {
    throw std::invalid_argument("The provided points do not form a rectangle.");
  }

  _width = width1;
  _height = height2;
}

Rectangle::Rectangle(const Rectangle &other)
    : Figure(other), _width(other._width), _height(other._height) {}

Rectangle::Rectangle(Rectangle &&other) noexcept
    : Figure(std::move(other)), _width(other._width), _height(other._height) {
  other._width = 0;
  other._height = 0;
}

Rectangle &Rectangle::operator=(const Rectangle &other) {
  if (this != &other) {
    Figure::operator=(other);
    _width = other._width;
    _height = other._height;
  }
  return *this;
}

Rectangle &Rectangle::operator=(Rectangle &&other) noexcept {
  if (this != &other) {
    Figure::operator=(std::move(other));
    _width = other._width;
    _height = other._height;
    other._width = 0;
    other._height = 0;
  }
  return *this;
}

Rectangle::operator double() const { return _width * _height; }

// TRAPEZOID REALIZATION

bool Trapezoid::is_collinear(const Point &v1, const Point &v2) {
  return std::abs(v1.getX() * v2.getY() - v1.getY() * v2.getX()) <
         std::numeric_limits<double>::epsilon();
}

bool Trapezoid::are_equal(const Point &v1, const Point &v2) {
  return std::abs(v1.getX() - v2.getX()) <
             std::numeric_limits<double>::epsilon() &&
         std::abs(v1.getY() - v2.getY()) <
             std::numeric_limits<double>::epsilon();
}

double Trapezoid::distance(const Point &p1, const Point &p2) {
  return std::sqrt(std::pow(p1.getX() - p2.getX(), 2) +
                   std::pow(p1.getY() - p2.getY(), 2));
}

double Trapezoid::triangleArea(const Point &a, const Point &b, const Point &c) {
  return 0.5 * std::abs((b.getX() - a.getX()) * (c.getY() - a.getY()) -
                        (c.getX() - a.getX()) * (b.getY() - a.getY()));
}

Trapezoid::Trapezoid(const std::initializer_list<Point> &lst) : Figure(lst) {
  if (lst.size() != 4) {
    throw std::invalid_argument("Trapezoid must have 4 points");
  }

  int p1 = -1, p2 = -1;
  for (int i = 0; i < 4; ++i) {
    for (int j = i + 1; j < 4; ++j) {
      Point v1(_points[(i + 1) % 4].getX() - _points[i].getX(),
               _points[(i + 1) % 4].getY() - _points[i].getY());
      Point v2(_points[(j + 1) % 4].getX() - _points[j].getX(),
               _points[(j + 1) % 4].getY() - _points[j].getY());

      if (is_collinear(v1, v2) && !are_equal(v1, v2)) {
        p1 = i;
        p2 = j;
        break;
      }
    }
    if (p1 != -1)
      break;
  }

  if (p1 == -1) {
    throw std::invalid_argument("The provided points do not form a trapezoid.");
  }

  _base1 = distance(_points[p1], _points[(p1 + 1) % 4]);
  _base2 = distance(_points[p2], _points[(p2 + 1) % 4]);

  double area1 = triangleArea(_points[0], _points[1], _points[2]);
  double area2 = triangleArea(_points[0], _points[2], _points[3]);
  _height = (area1 + area2) * 2 / (_base1 + _base2);
}

Trapezoid::Trapezoid(const Trapezoid &other)
    : Figure(other), _base1(other._base1), _base2(other._base2),
      _height(other._height) {}

Trapezoid::Trapezoid(Trapezoid &&other) noexcept
    : Figure(std::move(other)), _base1(other._base1), _base2(other._base2),
      _height(other._height) {
  other._base1 = 0;
  other._base2 = 0;
  other._height = 0;
}

Trapezoid &Trapezoid::operator=(const Trapezoid &other) {
  if (this != &other) {
    Figure::operator=(other);
    _base1 = other._base1;
    _base2 = other._base2;
    _height = other._height;
  }
  return *this;
}

Trapezoid &Trapezoid::operator=(Trapezoid &&other) noexcept {
  if (this != &other) {
    Figure::operator=(std::move(other));
    _base1 = other._base1;
    _base2 = other._base2;
    _height = other._height;
    other._base1 = 0;
    other._base2 = 0;
    other._height = 0;
  }
  return *this;
}

Trapezoid::operator double() const { return 0.5 * (_base1 + _base2) * _height; }
