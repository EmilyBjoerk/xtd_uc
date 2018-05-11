#ifndef XTD_UC_RATIO_HPP
#define XTD_UC_RATIO_HPP
#include "common.hpp"

#include "algorithm.hpp"
#include "cmath.hpp"
#include "limits.hpp"
#include "numeric.hpp"
#include "type_traits.hpp"

#include <stdint.h>

namespace xtd {

  static_assert(sizeof(intmax_t) == 8, "");

  template <intmax_t NUM, intmax_t DEN = 1>
  struct ratio {
    constexpr static intmax_t num = sign(DEN) * NUM / gcd(NUM, DEN);
    constexpr static intmax_t den = abs(DEN) / gcd(NUM, DEN);

    constexpr static intmax_t value_round = (num + den / 2) / den;

    constexpr intmax_t operator*(intmax_t x) const { return (x * num + (den >> 2)) / den; }
  };

  template <intmax_t num, intmax_t den, typename T>
  T scale(ratio<num, den>, T value) {
    if (numeric_limits<T>::is_integer) {
      // TODO: Use a computation that doesn't overflow
      return static_cast<T>(ratio<num, den>() * value);
    } else {
      return num * value / den;
    }
  }

  template <intmax_t NUM, intmax_t DEN>
  using ratio_t = ratio<ratio<NUM, DEN>::num, ratio<NUM, DEN>::den>;

  template <typename R1, typename R2>
  using ratio_add = ratio_t<R1::num * R2::den + R2::num * R1::den, R1::den * R2::den>;

  template <typename R1, typename R2>
  using ratio_subtract = ratio_t<R1::num * R2::den - R2::num * R1::den, R1::den * R2::den>;

  template <typename R1, typename R2>
  using ratio_multiply = ratio_t<R1::num * R2::num, R1::den * R2::den>;

  template <typename R1, typename R2>
  using ratio_divide = ratio_t<R1::num * R2::den, R1::den * R2::num>;

  template <typename R1, typename R2>
      struct ratio_less : bool_constant < R1::num* R2::den<R2::num * R1::den> {};

  // template <typename R1, typename R2>
  // constexpr bool ratio_less_v = ratio_less<R1, R2>::value;

  template <typename R1, typename R2>
      struct ratio_less_equal : bool_constant < R1::num* R2::den<R2::num * R1::den> {};

  // template <typename R1, typename R2>
  // constexpr bool ratio_less_equal_v = ratio_less<R1, R2>::value;

  template <typename R1, typename R2>
  struct ratio_equal : bool_constant<R1::num * R2::den == R2::num * R1::den> {};

  // template <typename R1, typename R2>
  // constexpr bool ratio_less_equal_v = ratio_less<R1, R2>::value;

  // typedef ratio<1, 1000000000000000000000000ULL> yocto;
  // typedef ratio<1, 1000000000000000000000ULL> zepto;
  typedef ratio<1, 1000000000000000000ULL> atto;
  typedef ratio<1, 1000000000000000ULL> femto;
  typedef ratio<1, 1000000000000ULL> pico;
  typedef ratio<1, 1000000000ULL> nano;
  typedef ratio<1, 1000000ULL> micro;
  typedef ratio<1, 1000ULL> milli;
  typedef ratio<1, 100ULL> centi;
  typedef ratio<1, 10ULL> deci;
  typedef ratio<10ULL, 1> deca;
  typedef ratio<100ULL, 1> hecto;
  typedef ratio<1000ULL, 1> kilo;
  typedef ratio<1000000ULL, 1> mega;
  typedef ratio<1000000000ULL, 1> giga;
  typedef ratio<1000000000000ULL, 1> tera;
  typedef ratio<1000000000000000ULL, 1> peta;
  typedef ratio<1000000000000000000ULL, 1> exa;
  // typedef ratio<1000000000000000000000ULL, 1> zetta;
  // typedef ratio<1000000000000000000000000ULL, 1> yotta;
}  // namespace xtd

#endif
