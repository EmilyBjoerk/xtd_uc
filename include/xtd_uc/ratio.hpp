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
  };

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

  template <typename R1>
  using ratio_invert = ratio_t<R1::den, R1::num>;

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

  namespace si_prefixes {
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
  }  // namespace si_prefixes

  using namespace si_prefixes;

  // computes x*r where r is a ratio<> object.
  template <typename R, typename T,
            round_style rounding = round_style::truncate>  // todo; enable only for r is ratio
  constexpr auto
  ratio_scale(T value) {
    if (numeric_limits<T>::is_integer) {
      // todo: use a computation that doesn't overflow
      if (rounding == round_style::nearest) {
        auto p = value * R::num;
        return (p + sign(p) * (R::den / 2)) / R::den;
      } else if (rounding == round_style::truncate) {
        return value * R::num / R::den;
      } else if (rounding == round_style::floor) {
        return (value * R::num - ((R::den + 1) / 2)) / R::den;
      } else {  // ceil
        return (value * R::num + R::den - 1) / R::den;
      }
    } else {
      return R::num * value / R::den;
    }
  }

  // given a value x and two ratios, r_left and r_right, let: y * r_left = x * r_right
  // this function computes 'y' such tht the above expression holds.
  template <typename r_left, typename r_right, typename T,
            round_style rounding = round_style::truncate>
  constexpr T ratio_convert(T x) {
    using scale = ratio_divide<r_right, r_left>;
    return ratio_scale<scale, T, rounding>(x);
  }

}  // namespace xtd

#endif
