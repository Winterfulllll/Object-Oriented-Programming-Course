#include "src/solution.h"
#include <iostream>

int main() {
  Five a{'1', '4', '3'};
  Five b("24");

  std::cout << "a = " << a << std::endl; // 143
  std::cout << "b = " << b << std::endl; // 24

  Five c = a + b;
  std::cout << "a + b = " << c << std::endl; // 222

  c = a - b;
  std::cout << "a - b = " << c << std::endl; // 114

  return 0;
}
