#pragma once

#include <initializer_list>
#include <iostream>

class Five {
public:
  Five();
  Five(const size_t &n, unsigned char t = 0);
  Five(const std::initializer_list<unsigned char> &lst);
  Five(const std::string &str);
  Five(const Five &other);
  Five(Five &&other) noexcept;
  ~Five() noexcept;

  Five &operator=(const Five &other);
  Five &operator=(Five &&other) noexcept;
  Five &operator+=(const Five &other);
  Five &operator-=(const Five &other);
  friend Five operator+(const Five &a, const Five &b);
  friend Five operator-(const Five &a, const Five &b);

  friend bool operator==(const Five &a, const Five &b);
  friend bool operator!=(const Five &a, const Five &b);
  friend bool operator<(const Five &a, const Five &b);
  friend bool operator>(const Five &a, const Five &b);
  friend bool operator<=(const Five &a, const Five &b);
  friend bool operator>=(const Five &a, const Five &b);

  friend std::ostream &operator<<(std::ostream &os, const Five &num);

private:
  bool isInvalidDigit(unsigned char c);
  bool
  isValidFiveInitListOfStr(const std::initializer_list<unsigned char> &lst);
  bool isValidFiveString(const std::string &str);

private:
  size_t _sz;
  unsigned char *_arr;
};
