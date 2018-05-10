#ifndef XTD_UC_UTILITY_HPP
#define XTD_UC_UTILITY_HPP
#include "common.hpp"

#include <stdint.h>
#include "type_traits.hpp"

namespace xtd {
  template <typename T>
  constexpr void swap(T& a, T& b) {
    T c{a};
    a = b;
    b = c;
  }

  inline void set_bit(volatile uint8_t& sfr, int bit) {
    sfr = static_cast<uint8_t>(sfr | (1 << bit));
  }

  inline void clr_bit(volatile uint8_t& sfr, int bit) {
    sfr = static_cast<uint8_t>(sfr & ~(1 << bit));
  }

  // Computes X * U / V without overflow assuming GCD(U,V) == 1
  template <typename T, typename U, typename V>
  auto integer_scale(T x, U num, V den) {
    int signs = 0;
    if (x < 0) {
      signs++;
      x = -x;
    }
    if (num < 0) {
      signs++;
      num = -num;
    }
    if (den < 0) {
      signs++;
      den = -den;
    }
    T ans;
    if (num < x) {
      ans = integer_scale(x, num, 0, den);
    } else {
      ans = integer_scale(num, x, 0, den);
    }
    if (signs & 0x1) {
      ans = -ans;
    }
    return ans;
  }

  // Recursively computes (a*b + c)/den without overflow (if the result fits).
  // a, b, c and den have to be positive integers
  template <typename T>
  auto integer_scale(T a, T b, T c, T den) {
    if (b == 0) {
      return c / den;
    }

    // (a*b + c) / den
    // Minimize b by computing the long multiplication partially
    while (a + c > c) {
      if (b & 0x1) {
        c += a;
      }
      b >>= 1;
      a <<= 1;
    }

    // (a*b + c) / den = a/den*b + c/den = qa*b + ra*b/den + qc + rc/den = qa*b + qc + (ra*b +
    // rc)/den
    auto qa = a / den;
    auto ra = a % den;
    auto qc = c / den;
    auto rc = c % den;

    return qa * b + qc + integer_scale(ra, b, rc, den);
  }

  // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2951.html
  template <class T, class U,
            class = typename enable_if<
                (is_lvalue_reference<T>::value ? is_lvalue_reference<U>::value : true) /*&&
                is_convertible<typename remove_reference<U>::type*,
                typename remove_reference<T>::type*>::value*/>::type>
  inline T&& forward(U&& u) {
    return static_cast<T&&>(u);
  }

  template <class T>
  typename add_rvalue_reference<T>::type declval() noexcept;
}  // namespace xtd

#endif
