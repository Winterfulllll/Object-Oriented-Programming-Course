#include <cctype>
#include <iostream>
#include <string>

bool IsVowel(char c) {
   c = std::tolower(c);
   return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

std::string RemoveVowels(std::string s) {
   std::string result;
   for (char c : s) {
      if (!IsVowel(c)) {
         result += c;
      }
   }
   return result;
   std::cout << result << std::endl;
}

/*
int main() {
   std::string s;
   std::getline(std::cin, s);
   std::cout << RemoveVowels(s) << std::endl;
   return 0;
}
*/
