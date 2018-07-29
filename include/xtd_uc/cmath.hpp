#ifndef XTD_UC_MATH_HPP
#define XTD_UC_MATH_HPP

#include "common.hpp"
#include "limits.hpp"

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

  template <typename T, round_style rounding = round_style::truncate>
  constexpr T divide(T num, T den) {
    if (numeric_limits<T>::is_integer) {
      if (den < 0) {
        return divide<T, rounding>(-num, -den);
      }

      // todo: use a computation that doesn't overflow
      if (rounding == round_style::nearest) {
        auto d2 = den / 2;
        return (num + (num < 0 ? -d2 : d2)) / den;
      } else if (rounding == round_style::truncate) {
        return num / den;
      } else if (rounding == round_style::floor) {
        return (num - ((den + 1) / 2)) / den;
      } else {  // ceil
        return (num + den - 1) / den;
      }
    } else {
      return num / den;
    }
  }
}  // namespace xtd

#endif
