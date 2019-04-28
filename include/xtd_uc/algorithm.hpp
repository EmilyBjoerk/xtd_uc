#ifndef XTD_UC_ALGORITHM_HPP
#define XTD_UC_ALGORITHM_HPP
#include "common.hpp"

namespace xtd {
  template <class T>
  constexpr const T& max(const T& a, const T& b) {
    return a < b ? b : a;
  }

  template <class T>
  constexpr const T& min(const T& a, const T& b) {
    return a < b ? a : b;
  }

  template <typename T>
  constexpr auto& clamp(const T& v, const T& min, const T& max) {
    return v < min ? min : max < v ? max : v;
  }
}  // namespace xtd

#endif
