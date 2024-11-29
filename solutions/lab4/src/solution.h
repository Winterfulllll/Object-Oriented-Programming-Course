#pragma once

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

template <typename T> concept Scalar = std::is_arithmetic<T>::value;

// POINT REALIZATION

template <Scalar T> class Point {
public:
  Point() = default;
  Point(T x, T y) : _x(x), _y(y){};
  Point(const Point<T> &other) = default;
  Point(Point<T> &&other) noexcept = default;
  ~Point() = default;

  T get_x() const { return _x; };
  T get_y() const { return _y; };

  Point<T> &operator=(const Point<T> &other);
  Point<T> &operator=(Point<T> &&other) noexcept;
  bool operator==(const Point<T> &other) const;

  template <Scalar U>
  friend std::ostream &operator<<(std::ostream &os, const Point<U> &p);
  template <Scalar U>
  friend std::istream &operator>>(std::istream &is, Point<U> &p);

private:
  T _x = 0.0;
  T _y = 0.0;
};

template <Scalar T> Point<T> &Point<T>::operator=(const Point<T> &other) {
  if (this != &other) {
    _x = other._x;
    _y = other._y;
  }
  return *this;
}

template <Scalar T> Point<T> &Point<T>::operator=(Point<T> &&other) noexcept {
  if (this != &other) {
    _x = other._x;
    _y = other._y;
  }
  return *this;
}

template <Scalar T> bool Point<T>::operator==(const Point<T> &other) const {
  if constexpr (std::is_floating_point_v<T>) {
    return std::abs(_x - other._x) < std::numeric_limits<T>::epsilon() &&
           std::abs(_y - other._y) < std::numeric_limits<T>::epsilon();
  } else {
    return _x == other._x && _y == other._y;
  }
}

template <Scalar T>
std::ostream &operator<<(std::ostream &os, const Point<T> &p) {
  os << "(" << p.get_x() << ", " << p.get_y() << ")";
  return os;
}

template <Scalar T> std::istream &operator>>(std::istream &is, Point<T> &p) {
  T x, y;
  is >> x >> y;
  p = Point<T>(x, y);
  return is;
}

// FIGURE REALIZATION

template <Scalar T> class Figure {
public:
  Figure() = default;
  Figure(const std::initializer_list<Point<T>> &lst);
  Figure(const Figure<T> &other);
  Figure(Figure<T> &&other) noexcept = default;
  virtual ~Figure() noexcept = default;

  Figure<T> &operator=(const Figure<T> &other);
  Figure<T> &operator=(Figure<T> &&other) noexcept = default;
  bool operator==(const Figure<T> &other);

  Point<T> get_barycenter() const;
  size_t get_size() const;
  virtual explicit operator double() const;

  template <Scalar U>
  friend std::ostream &operator<<(std::ostream &os, const Figure<U> &fig);
  template <Scalar U>
  friend std::istream &operator>>(std::istream &is, Figure<U> &fig);

protected:
  std::vector<std::unique_ptr<Point<T>>> _points;
};

template <Scalar T>
Figure<T>::Figure(const std::initializer_list<Point<T>> &lst) {
  if (lst.size() < 3) {
    throw std::runtime_error("Figure must have at least 3 points");
  }
  _points.reserve(lst.size());
  for (const auto &p : lst) {
    _points.push_back(std::make_unique<Point<T>>(p));
  }
}

template <Scalar T> Figure<T>::Figure(const Figure<T> &other) {
  _points.reserve(other._points.size());
  for (const auto &p : other._points) {
    _points.push_back(std::make_unique<Point<T>>(*p));
  }
}

template <Scalar T> Figure<T> &Figure<T>::operator=(const Figure<T> &other) {
  if (this != &other) {
    _points.clear();
    _points.reserve(other._points.size());
    for (const auto &p : other._points) {
      _points.push_back(std::make_unique<Point<T>>(*p));
    }
  }
  return *this;
}

template <Scalar T> bool Figure<T>::operator==(const Figure<T> &other) {
  if (_points.size() != other._points.size()) {
    return false;
  }
  for (size_t i = 0; i < _points.size(); ++i) {
    if (*_points[i] != *other._points[i]) {
      return false;
    }
  }
  return true;
}

template <Scalar T> Point<T> Figure<T>::get_barycenter() const {
  double sum_x = 0;
  double sum_y = 0;
  for (const auto &p : _points) {
    sum_x += p->get_x();
    sum_y += p->get_y();
  }
  return Point<T>(sum_x / _points.size(), sum_y / _points.size());
}

template <Scalar T> size_t Figure<T>::get_size() const {
  return this->_points.size();
}

template <Scalar T> Figure<T>::operator double() const {
  double area = 0.0;
  for (size_t i = 0; i < _points.size(); ++i) {
    size_t j = (i + 1) % _points.size();
    area += (_points[i]->get_x() * _points[j]->get_y() -
             _points[j]->get_x() * _points[i]->get_y());
  }
  return std::abs(area) / 2.0;
}

template <Scalar T>
std::ostream &operator<<(std::ostream &os, const Figure<T> &fig) {
  os << "Figure with " << fig._points.size() << " points: {";
  for (size_t i = 0; i < fig._points.size(); ++i) {
    os << *fig._points[i] << (i < fig._points.size() - 1 ? ", " : "");
  }
  os << "}";
  return os;
}

template <Scalar T> std::istream &operator>>(std::istream &is, Figure<T> &fig) {
  std::cout << "Enter the number of points: ";
  size_t points_count;
  is >> points_count;

  if (points_count < 3) {
    throw std::runtime_error("Figure must have at least 3 points");
  }

  fig._points.clear();
  fig._points.reserve(points_count);

  for (size_t i = 0; i < points_count; ++i) {
    std::cout << "Enter point " << i + 1 << " (x y): ";
    Point<T> p;
    is >> p;
    fig._points.push_back(std::make_unique<Point<T>>(p));
  }
  return is;
}

// SQUARE REALIZATION

template <Scalar T> class Square : public Figure<T> {
public:
  Square(Point<T> start, double side);
  Square(const std::initializer_list<Point<T>> &lst);

  Square(const Square<T> &other);
  Square(Square<T> &&other) noexcept;
  Square<T> &operator=(const Square<T> &other);
  Square<T> &operator=(Square<T> &&other) noexcept;

  T get_side() const { return _side; };
  operator double() const override;

private:
  T _side;
};

template <Scalar T>
Square<T>::Square(Point<T> start, double side) : _side(side) {
  if (side <= 0) {
    throw std::invalid_argument("Side length must be positive");
  }

  this->_points.reserve(4);
  this->_points.push_back(std::make_unique<Point<T>>(start));
  this->_points.push_back(
      std::make_unique<Point<T>>(start.get_x() + side, start.get_y()));
  this->_points.push_back(
      std::make_unique<Point<T>>(start.get_x() + side, start.get_y() + side));
  this->_points.push_back(
      std::make_unique<Point<T>>(start.get_x(), start.get_y() + side));
}

template <Scalar T>
Square<T>::Square(const std::initializer_list<Point<T>> &lst) : Figure<T>(lst) {
  if (lst.size() != 4) {
    throw std::invalid_argument("Square must have 4 points");
  }

  auto it = lst.begin();
  auto it1 = it + 1;
  auto it2 = it + 2;

  double side1 = std::sqrt(std::pow(it->get_x() - it1->get_x(), 2) +
                           std::pow(it->get_y() - it1->get_y(), 2));
  double side2 = std::sqrt(std::pow(it1->get_x() - it2->get_x(), 2) +
                           std::pow(it1->get_y() - it2->get_y(), 2));

  if (std::abs(side1 - side2) > std::numeric_limits<double>::epsilon()) {
    throw std::invalid_argument("The provided points do not form a square");
  }

  _side = side1;
}

template <Scalar T>
Square<T>::Square(const Square<T> &other)
    : Figure<T>(other), _side(other._side) {}

template <Scalar T>
Square<T>::Square(Square<T> &&other) noexcept
    : Figure<T>(std::move(other)), _side(other._side) {
  other._side = 0;
}

template <Scalar T> Square<T> &Square<T>::operator=(const Square<T> &other) {
  if (this != &other) {
    Figure<T>::operator=(other);
    _side = other._side;
  }
  return *this;
}

template <Scalar T>
Square<T> &Square<T>::operator=(Square<T> &&other) noexcept {
  if (this != &other) {
    Figure<T>::operator=(std::move(other));
    _side = other._side;
    other._side = 0;
  }
  return *this;
}

template <Scalar T> Square<T>::operator double() const { return _side * _side; }

// RECTANGLE REALIZATION

template <Scalar T> class Rectangle : public Figure<T> {
public:
  Rectangle(Point<T> start, double width, double height);
  Rectangle(const std::initializer_list<Point<T>> &lst);

  Rectangle(const Rectangle<T> &other);
  Rectangle(Rectangle<T> &&other) noexcept;
  Rectangle<T> &operator=(const Rectangle<T> &other);
  Rectangle<T> &operator=(Rectangle<T> &&other) noexcept;

  T get_width() const { return _width; };
  T get_height() const { return _height; };
  operator double() const override;

private:
  T _width, _height;
};

template <Scalar T>
Rectangle<T>::Rectangle(Point<T> start, double width, double height)
    : _width(width), _height(height) {
  if (width <= 0 || height <= 0) {
    throw std::invalid_argument("Width and height must be positive");
  }

  this->_points.reserve(4);
  this->_points.push_back(std::make_unique<Point<T>>(start));
  this->_points.push_back(
      std::make_unique<Point<T>>(start.get_x() + width, start.get_y()));
  this->_points.push_back(std::make_unique<Point<T>>(start.get_x() + width,
                                                     start.get_y() + height));
  this->_points.push_back(
      std::make_unique<Point<T>>(start.get_x(), start.get_y() + height));
}

template <Scalar T>
Rectangle<T>::Rectangle(const std::initializer_list<Point<T>> &lst)
    : Figure<T>(lst) {
  if (lst.size() != 4) {
    throw std::invalid_argument("Rectangle must have 4 points");
  }

  auto it = lst.begin();
  auto it1 = it + 1;
  auto it2 = it + 2;
  auto it3 = it + 3;

  double width1 = std::sqrt(std::pow(it->get_x() - it1->get_x(), 2) +
                            std::pow(it->get_y() - it1->get_y(), 2));
  double height1 = std::sqrt(std::pow(it1->get_x() - it2->get_x(), 2) +
                             std::pow(it1->get_y() - it2->get_y(), 2));
  double width2 = std::sqrt(std::pow(it2->get_x() - it3->get_x(), 2) +
                            std::pow(it2->get_y() - it3->get_y(), 2));
  double height2 = std::sqrt(std::pow(it3->get_x() - it->get_x(), 2) +
                             std::pow(it3->get_y() - it->get_y(), 2));

  if (std::abs(width1 - width2) > std::numeric_limits<double>::epsilon() ||
      std::abs(height1 - height2) > std::numeric_limits<double>::epsilon()) {
    throw std::invalid_argument("The provided points do not form a rectangle.");
  }

  _width = width1;
  _height = height2;
}

template <Scalar T>
Rectangle<T>::Rectangle(const Rectangle<T> &other)
    : Figure<T>(other), _width(other._width), _height(other._height) {}

template <Scalar T>
Rectangle<T>::Rectangle(Rectangle<T> &&other) noexcept
    : Figure<T>(std::move(other)), _width(other._width),
      _height(other._height) {
  other._width = 0;
  other._height = 0;
}

template <Scalar T>
Rectangle<T> &Rectangle<T>::operator=(const Rectangle<T> &other) {
  if (this != &other) {
    Figure<T>::operator=(other);
    _width = other._width;
    _height = other._height;
  }
  return *this;
}

template <Scalar T>
Rectangle<T> &Rectangle<T>::operator=(Rectangle<T> &&other) noexcept {
  if (this != &other) {
    Figure<T>::operator=(std::move(other));
    _width = other._width;
    _height = other._height;
    other._width = 0;
    other._height = 0;
  }
  return *this;
}

template <Scalar T> Rectangle<T>::operator double() const {
  return _width * _height;
}

// TRAPEZOID REALIZATION

template <Scalar T> class Trapezoid : public Figure<T> {
public:
  Trapezoid(const Point<T> &a, const Point<T> &b, const Point<T> &c,
            const Point<T> &d);
  Trapezoid(const std::initializer_list<Point<T>> &lst);

  Trapezoid(const Trapezoid<T> &other);
  Trapezoid(Trapezoid<T> &&other) noexcept;
  Trapezoid<T> &operator=(const Trapezoid<T> &other);
  Trapezoid<T> &operator=(Trapezoid<T> &&other) noexcept;

  T get_base1() const { return _base1; };
  T get_base2() const { return _base2; };
  T get_height() const { return _height; };
  operator double() const override;

private:
  static bool is_collinear(const Point<T> &v1, const Point<T> &v2);
  static bool are_equal(const Point<T> &v1, const Point<T> &v2);
  static double distance(const Point<T> &p1, const Point<T> &p2);
  static double triangleArea(const Point<T> &a, const Point<T> &b,
                             const Point<T> &c);

private:
  T _base1, _base2, _height;
};

template <Scalar T>
bool Trapezoid<T>::is_collinear(const Point<T> &v1, const Point<T> &v2) {
  return std::abs(v1.get_x() * v2.get_y() - v1.get_y() * v2.get_x()) <
         std::numeric_limits<double>::epsilon();
}

template <Scalar T>
bool Trapezoid<T>::are_equal(const Point<T> &v1, const Point<T> &v2) {
  return std::abs(v1.get_x() - v2.get_x()) <
             std::numeric_limits<double>::epsilon() &&
         std::abs(v1.get_y() - v2.get_y()) <
             std::numeric_limits<double>::epsilon();
}

template <Scalar T>
double Trapezoid<T>::distance(const Point<T> &p1, const Point<T> &p2) {
  return std::sqrt(std::pow(p1.get_x() - p2.get_x(), 2) +
                   std::pow(p1.get_y() - p2.get_y(), 2));
}

template <Scalar T>
double Trapezoid<T>::triangleArea(const Point<T> &a, const Point<T> &b,
                                  const Point<T> &c) {
  return 0.5 * std::abs((b.get_x() - a.get_x()) * (c.get_y() - a.get_y()) -
                        (c.get_x() - a.get_x()) * (b.get_y() - a.get_y()));
}

template <Scalar T>
Trapezoid<T>::Trapezoid(const std::initializer_list<Point<T>> &lst)
    : Figure<T>(lst) {
  if (lst.size() != 4) {
    throw std::invalid_argument("Trapezoid must have 4 points");
  }

  int p1 = -1, p2 = -1;
  for (int i = 0; i < 4; ++i) {
    for (int j = i + 1; j < 4; ++j) {
      Point<T> v1(
          this->_points[(i + 1) % 4]->get_x() - this->_points[i]->get_x(),
          this->_points[(i + 1) % 4]->get_y() - this->_points[i]->get_y());
      Point<T> v2(
          this->_points[(j + 1) % 4]->get_x() - this->_points[j]->get_x(),
          this->_points[(j + 1) % 4]->get_y() - this->_points[j]->get_y());

      if (is_collinear(v1, v2) && !are_equal(v1, v2)) {
        p1 = i;
        p2 = j;
        break;
      }
    }
    if (p1 != -1) {
      break;
    }
  }

  if (p1 == -1) {
    throw std::invalid_argument("The provided points do not form a trapezoid.");
  }

  _base1 = distance(*this->_points[p1], *this->_points[(p1 + 1) % 4]);
  _base2 = distance(*this->_points[p2], *this->_points[(p2 + 1) % 4]);

  double area1 =
      triangleArea(*this->_points[0], *this->_points[1], *this->_points[2]);
  double area2 =
      triangleArea(*this->_points[0], *this->_points[2], *this->_points[3]);
  _height = (area1 + area2) * 2 / (_base1 + _base2);
}

template <Scalar T>
Trapezoid<T>::Trapezoid(const Point<T> &a, const Point<T> &b, const Point<T> &c,
                        const Point<T> &d)
    : Figure<T>({a, b, c, d}) {
  if (this->_points.size() != 4) { // Проверка
    throw std::invalid_argument("Trapezoid must have 4 points");
  }

  int p1 = -1, p2 = -1;
  for (int i = 0; i < 4; ++i) {
    for (int j = i + 1; j < 4; ++j) {
      Point<T> v1(
          this->_points[(i + 1) % 4]->get_x() - this->_points[i]->get_x(),
          this->_points[(i + 1) % 4]->get_y() - this->_points[i]->get_y());
      Point<T> v2(
          this->_points[(j + 1) % 4]->get_x() - this->_points[j]->get_x(),
          this->_points[(j + 1) % 4]->get_y() - this->_points[j]->get_y());

      if (is_collinear(v1, v2) && !are_equal(v1, v2)) {
        p1 = i;
        p2 = j;
        break;
      }
    }
    if (p1 != -1) {
      break;
    }
  }

  if (p1 == -1) {
    throw std::invalid_argument("The provided points do not form a trapezoid.");
  }

  _base1 = distance(*this->_points[p1], *this->_points[(p1 + 1) % 4]);
  _base2 = distance(*this->_points[p2], *this->_points[(p2 + 1) % 4]);

  double area1 =
      triangleArea(*this->_points[0], *this->_points[1], *this->_points[2]);
  double area2 =
      triangleArea(*this->_points[0], *this->_points[2], *this->_points[3]);

  if (std::abs(_base1 + _base2) < std::numeric_limits<double>::epsilon()) {
    throw std::logic_error("Sum of bases cannot be near zero");
  }

  _height = (area1 + area2) * 2 / (_base1 + _base2);
}

template <Scalar T>
Trapezoid<T>::Trapezoid(const Trapezoid<T> &other)
    : Figure<T>(other), _base1(other._base1), _base2(other._base2),
      _height(other._height) {}

template <Scalar T>
Trapezoid<T>::Trapezoid(Trapezoid<T> &&other) noexcept
    : Figure<T>(std::move(other)), _base1(other._base1), _base2(other._base2),
      _height(other._height) {
  other._base1 = 0;
  other._base2 = 0;
  other._height = 0;
}

template <Scalar T>
Trapezoid<T> &Trapezoid<T>::operator=(const Trapezoid<T> &other) {
  if (this != &other) {
    Figure<T>::operator=(other);
    _base1 = other._base1;
    _base2 = other._base2;
    _height = other._height;
  }
  return *this;
}

template <Scalar T>
Trapezoid<T> &Trapezoid<T>::operator=(Trapezoid<T> &&other) noexcept {
  if (this != &other) {
    Figure<T>::operator=(std::move(other));
    _base1 = other._base1;
    _base2 = other._base2;
    _height = other._height;
    other._base1 = 0;
    other._base2 = 0;
    other._height = 0;
  }
  return *this;
}

template <Scalar T> Trapezoid<T>::operator double() const {
  return 0.5 * (_base1 + _base2) * _height;
}
