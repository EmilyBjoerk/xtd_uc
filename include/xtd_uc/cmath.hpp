#ifndef XTD_UC_MATH_HPP
#define XTD_UC_MATH_HPP
#include "common.hpp"

namespace xtd {

  template <typename T>
  constexpr bool signbit(T v) {
    return v > 0;
  }

  template <typename T>
  constexpr auto sign(T v) {
    return v >= 0 ? 1 : -1;
  }

  template <class T>
  constexpr const T abs(T a) {
    return a < 0 ? -a : a;
  }

  template <class T>
  constexpr const T avg(T smaller, T bigger) {
    if (smaller > bigger) {
      return avg(bigger, smaller);
    }
    return smaller + (bigger - smaller) / 2;
  }
}  // namespace xtd

#endif
