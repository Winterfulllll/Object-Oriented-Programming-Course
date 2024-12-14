#include "src/solution.h"
#include <iostream>

const int MAX_FIGURES = 100;

int main() {
  Figure *figures[MAX_FIGURES];
  int figureCount = 0;
  double totalArea = 0;
  char figureType;

  while (true) {
    std::cout << "Enter figure type (s - square, r - rectangle, t - trapezoid, "
                 "e - exit, p - print, d - delete): ";
    std::cin >> figureType;

    if (figureType == 'e') {
      break;

    } else if (figureType == 'p') {
      for (int i = 0; i < figureCount; ++i) {
        std::cout << "Figure " << i + 1 << ": " << *figures[i]
                  << ", Barycenter: " << figures[i]->get_barycenter()
                  << ", Area: " << static_cast<double>(*figures[i]) << '\n';
        totalArea += static_cast<double>(*figures[i]);
      }
      std::cout << "Total area: " << totalArea << '\n';
      totalArea = 0;
      continue;

    } else if (figureType == 'd') {
      int index;
      std::cout << "Enter index to delete: ";
      std::cin >> index;

      if (index < 0 || index >= figureCount) {
        std::cout << "Invalid index." << '\n';
      } else {
        delete figures[index];
        for (int i = index; i < figureCount - 1; ++i) {
          figures[i] = figures[i + 1];
        }
        figureCount--;
      }
      continue;
    }

    if (figureCount >= MAX_FIGURES) {
      std::cout << "Maximum number of figures reached." << '\n';
      continue;
    }

    try {
      if (figureType == 's') {
        Point start;
        double side;
        std::cout << "Enter start point (x0 y0): ";
        std::cin >> start;
        std::cout << "Enter side length: ";
        std::cin >> side;
        figures[figureCount] = new Square(start, side);

      } else if (figureType == 'r') {
        Point start;
        double width, height;
        std::cout << "Enter start point (x0 y0): ";
        std::cin >> start;
        std::cout << "Enter width and height: ";
        std::cin >> width >> height;
        figures[figureCount] = new Rectangle(start, width, height);

      } else if (figureType == 't') {
        Point p1, p2, p3, p4;
        std::cout << "Enter 4 trapezoid points (x1 y1 x2 y2 x3 y3 x4 y4): ";
        std::cin >> p1 >> p2 >> p3 >> p4;
        figures[figureCount] = new Trapezoid({p1, p2, p3, p4});

      } else {
        std::cout << "Invalid figure type." << '\n';
        continue;
      }
      figureCount++;

    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << '\n';
    }
  }

  for (int i = 0; i < figureCount; ++i) {
    delete figures[i];
  }

  return 0;
}
