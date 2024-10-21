#include "src/solution.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

template <typename T> class Array {
private:
  std::shared_ptr<T[]> data;
  size_t size;
  size_t capacity;

public:
  Array() : data(nullptr), size(0), capacity(0) {}
  Array(size_t capacity) : size(0), capacity(capacity) {
    data = std::shared_ptr<T[]>(new T[capacity]);
  }

  void push_back(const T &value) {
    if (size >= capacity) {
      size_t newCapacity = capacity == 0 ? 1 : capacity * 2;
      std::shared_ptr<T[]> newData = std::shared_ptr<T[]>(new T[newCapacity]);
      for (size_t i = 0; i < size; ++i) {
        newData[i] = std::move(data[i]);
      }
      data = newData;
      capacity = newCapacity;
    }
    data[size++] = value;
  }

  void erase(size_t index) {
    if (index < size) {
      for (size_t i = index; i < size - 1; ++i) {
        data[i] = std::move(data[i + 1]);
      }
      size--;
    }
  }

  T &operator[](size_t index) { return data[index]; }
  const T &operator[](size_t index) const { return data[index]; }

  size_t get_size() const { return size; }
  size_t get_capacity() const { return capacity; }
};

int main() {
  // Example with Array<Figure<int>*>
  Array<std::shared_ptr<Figure<int>>> figures_int;
  figures_int.push_back(std::make_shared<Square<int>>(Point<int>(0, 0), 5));
  figures_int.push_back(
      std::make_shared<Rectangle<int>>(Point<int>(1, 1), 3, 4));

  // Example with Array<Square<double>>
  Array<std::shared_ptr<Square<double>>> squares_double;
  squares_double.push_back(
      std::make_shared<Square<double>>(Point<double>(2, 2), 2));

  // Example with runtime polymorphism and shared_ptr
  Array<std::shared_ptr<Figure<double>>> figures_double;

  while (true) {
    char figureType;
    std::cout << "Enter figure type (s - square, r - rectangle, t - trapezoid, "
                 "e - exit, p - print, d - delete): ";
    std::cin >> figureType;

    if (figureType == 'e') {
      break;
    } else if (figureType == 'p') {
      double totalArea = 0;
      for (size_t i = 0; i < figures_double.get_size(); ++i) {
        std::cout << "Figure " << i + 1 << ": " << *figures_double[i]
                  << ", Barycenter: " << figures_double[i]->get_barycenter()
                  << ", Area: " << static_cast<double>(*figures_double[i])
                  << '\n';
        totalArea += static_cast<double>(*figures_double[i]);
      }
      std::cout << "Total area: " << totalArea << '\n';
      continue;

    } else if (figureType == 'd') {
      size_t index;
      std::cout << "Enter index to delete: ";
      std::cin >> index;

      if (index < 1 || index >= figures_double.get_size()) {
        std::cout << "Invalid index." << '\n';
      } else {
        figures_double.erase(index - 1);
      }
      continue;
    }

    try {
      if (figureType == 's') {
        Point<double> start;
        double side;
        std::cout << "Enter start point (x0 y0): ";
        std::cin >> start;
        std::cout << "Enter side length: ";
        std::cin >> side;
        figures_double.push_back(std::make_shared<Square<double>>(start, side));

      } else if (figureType == 'r') {
        Point<double> start;
        double width, height;
        std::cout << "Enter start point (x0 y0): ";
        std::cin >> start;
        std::cout << "Enter width and height: ";
        std::cin >> width >> height;
        figures_double.push_back(
            std::make_shared<Rectangle<double>>(start, width, height));

      } else if (figureType == 't') {
        Point<double> p1, p2, p3, p4;
        std::cout << "Enter 4 trapezoid points (x1 y1 x2 y2 x3 y3 x4 y4): ";
        std::cin >> p1 >> p2 >> p3 >> p4;
        figures_double.push_back(
            std::make_shared<Trapezoid<double>>(p1, p2, p3, p4));

      } else {
        std::cout << "Invalid figure type." << '\n';
        continue;
      }

    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << '\n';
    }
  }

  return 0;
}
