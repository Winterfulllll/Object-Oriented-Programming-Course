#include <iostream>
#include <string>
#include "src/solution.h"

int main() {
   std::string s;
   std::getline(std::cin, s);
   std::cout << RemoveVowels(s) << std::endl;
   return 0;
}
