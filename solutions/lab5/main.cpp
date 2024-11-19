#include "src/solution.cpp"

struct MyStruct {
  int a;
  double b;
  std::string c;
};

int main() {
  DynamicMemoryResource mr;
  DynamicArray<int> arr{&mr};
  DynamicArray<MyStruct> structArr{&mr};

  for (int i = 0; i < 10; ++i) {
    arr.push_back(i);
  }

  for (int x : arr) {
    std::cout << x << " ";
  }
  std::cout << std::endl;

  structArr.push_back({1, 2.5, "hello"});
  structArr.push_back({2, 3.14, "world"});

  for (const auto &s : structArr) {
    std::cout << s.a << " " << s.b << " " << s.c << std::endl;
  }

  return 0;
}
