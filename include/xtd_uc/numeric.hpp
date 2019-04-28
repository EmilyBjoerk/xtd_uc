#ifndef XTD_UC_NUMERIC_HPP
#define XTD_UC_NUMERIC_HPP
#include "common.hpp"

#include "cmath.hpp"
#include "utility.hpp"

namespace xtd {
  template <typename M, typename N>
  constexpr auto gcd(M m, N n) {
    m = abs(m);
    n = abs(n);

    if (m == n) {
      return m;
    }
    if (m == 0) return n;
    if (n == 0) return m;

    // Remove common factors of 2
    int shift = 0;
    while (((m | n) & 1) == 0) {
      m >>= 1;
      n >>= 1;
      ++shift;
    }

    do {
      while ((n & 1) == 0) n >>= 1;

      if (m > n) {
        swap(m, n);
      }

      n = n - m;
    } while (n != 0);
    return m << shift;
  }
}  // namespace xtd

#endif
