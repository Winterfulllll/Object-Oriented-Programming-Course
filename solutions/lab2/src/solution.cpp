#include "solution.h"

#include <algorithm>
#include <stdexcept>

using uch = unsigned char;

bool Five::isInvalidDigit(uch c) { return c < '0' || c > '4'; }

bool Five::isValidFiveInitListOfStr(const std::initializer_list<uch> &lst) {
  for (char c : lst) {
    if (isInvalidDigit(c)) {
      return false;
    }
  }
  return true;
}

bool Five::isValidFiveString(const std::string &str) {
  for (char c : str) {
    if (isInvalidDigit(c)) {
      return false;
    }
  }
  return true;
}

Five::Five() : _sz(0), _arr(nullptr) {}

Five::Five(const size_t &n, uch t) : _sz(n), _arr(new uch[n]) {
  std::fill(this->_arr, this->_arr + this->_sz, t);
}

Five::Five(const std::initializer_list<unsigned char> &lst)
    : _sz(lst.size()), _arr(nullptr) {
  if (!isValidFiveInitListOfStr(lst)) {
    throw std::invalid_argument(
        "Invalid initializer_list for Five initialization.");
  }

  this->_arr = new uch[_sz];
  size_t i = 0;
  for (auto it = lst.begin(); it != lst.end(); ++it) {
    _arr[_sz - 1 - i++] = *it;
  }
}

Five::Five(const std::string &str) : _sz(str.size()), _arr(nullptr) {
  if (!isValidFiveString(str)) {
    throw std::invalid_argument("Invalid string for Five initialization.");
  }

  this->_arr = new uch[_sz];
  size_t i = 0;
  for (auto it = str.rbegin(); it != str.rend(); ++it) {
    _arr[i++] = *it;
  }
}

Five::Five(const Five &other) : _sz(other._sz), _arr(new uch[_sz]) {
  std::copy(other._arr, other._arr + _sz, this->_arr);
}

Five::Five(Five &&other) noexcept : _sz(other._sz), _arr(other._arr) {
  other._sz = 0;
  other._arr = nullptr;
}

Five::~Five() noexcept { delete[] _arr; }

Five &Five::operator=(const Five &other) {
  if (this != &other) {
    delete[] _arr;
    this->_sz = other._sz;
    this->_arr = new uch[_sz];
    std::copy(other._arr, other._arr + this->_sz, this->_arr);
  }
  return *this;
}

Five &Five::operator=(Five &&other) noexcept {
  if (this != &other) {
    delete[] _arr;
    this->_sz = other._sz;
    this->_arr = other._arr;
    other._sz = 0;
    other._arr = nullptr;
  }
  return *this;
}

Five &Five::operator+=(const Five &other) {
  size_t max_sz = std::max(_sz, other._sz) + 1;
  int *temp = new int[max_sz]();

  for (int i = 0; i < this->_sz; ++i) {
    temp[i] = this->_arr[i] - '0';
  }
  for (int i = 0; i < other._sz; ++i) {
    temp[i] += other._arr[i] - '0';
    if (temp[i] >= 5) {
      temp[i + 1] += temp[i] / 5;
      temp[i] %= 5;
    }
  }

  while (max_sz > 1 && temp[max_sz - 1] == 0) {
    --max_sz;
  }

  delete[] this->_arr;
  this->_sz = max_sz;
  this->_arr = new unsigned char[_sz];
  for (size_t i = 0; i < _sz; ++i) {
    _arr[i] = temp[i] + '0';
  }

  delete[] temp;
  return *this;
}

Five &Five::operator-=(const Five &other) {
  if (*this < other) {
    throw std::invalid_argument("Subtraction result is negative.");
  }

  size_t max_sz = std::max(_sz, other._sz);
  int *temp = new int[max_sz]();

  for (size_t i = 0; i < this->_sz; ++i) {
    temp[i] = this->_arr[i] - '0';
  }

  for (size_t i = 0; i < other._sz; ++i) {
    temp[i] -= other._arr[i] - '0';
    if (temp[i] < 0) {
      temp[i] += 5;
      temp[i + 1]--;
    }
  }

  while (max_sz > 1 && temp[max_sz - 1] == 0) {
    --max_sz;
  }

  delete[] this->_arr;
  this->_sz = max_sz;
  this->_arr = new unsigned char[_sz];
  for (size_t i = 0; i < _sz; ++i) {
    _arr[i] = temp[i] + '0';
  }

  delete[] temp;
  return *this;
}

Five operator+(const Five &a, const Five &b) {
  Five temp(a);
  temp += b;
  return temp;
}

Five operator-(const Five &a, const Five &b) {
  Five temp(a);
  temp -= b;
  return temp;
}

bool operator==(const Five &a, const Five &b) {
  if (a._sz != b._sz) {
    return false;
  }

  for (size_t i = 0; i < a._sz; ++i) {
    if (a._arr[i] != b._arr[i]) {
      return false;
    }
  }

  return true;
}

bool operator!=(const Five &a, const Five &b) { return !(a == b); }

bool operator<(const Five &a, const Five &b) {
  if (a._sz < b._sz) {
    return true;
  } else if (a._sz > b._sz) {
    return false;
  }

  for (int i = a._sz - 1; i >= 0; --i) {
    if (a._arr[i] < b._arr[i]) {
      return true;
    } else if (a._arr[i] > b._arr[i]) {
      return false;
    }
  }
  return false;
}

bool operator>(const Five &a, const Five &b) { return b < a; }

bool operator<=(const Five &a, const Five &b) { return !(a > b); }

bool operator>=(const Five &a, const Five &b) { return !(a < b); }

std::ostream &operator<<(std::ostream &os, const Five &num) {
  for (int i = num._sz - 1; i >= 0; --i) {
    os << num._arr[i];
  }
  return os;
}
