#ifndef XTD_UC_LIMITS_HPP
#define XTD_UC_LIMITS_HPP
#include "common.hpp"
#include "type_traits.hpp"

namespace xtd {
  enum float_denorm_style { denorm_indeterminate = -1, denorm_absent = 0, denorm_present = 1 };
  enum float_round_style {
    round_indeterminate = -1,
    round_toward_zero = 0,
    round_to_nearest = 1,
    round_toward_infinity = 2,
    round_toward_neg_infinity = 3
  };

  enum class round_style { nearest, truncate, floor, ceil };

  namespace detail {
    template <typename T>
    class numeric_limits_impl_int {
    public:
      constexpr static bool is_specialised = true;
      constexpr static bool is_signed = xtd::is_signed<T>::value;
      constexpr static bool is_integer = true;
      constexpr static bool is_exact = true;

      constexpr static bool has_infinity = false;
      constexpr static bool has_quiet_NaN = false;
      constexpr static bool has_signaling_NaN = false;
      constexpr static xtd::float_denorm_style has_denorm = xtd::float_denorm_style::denorm_absent;
      constexpr static bool has_denorm_loss = false;
      constexpr static xtd::float_round_style round_style = xtd::round_toward_zero;
      constexpr static bool is_iec559 = false;
      constexpr static bool is_bounded = true;
      constexpr static bool is_modulo = numeric_limits_impl_int<T>::is_signed ? false : true;
      constexpr static int digits = 8 * sizeof(T) - (numeric_limits_impl_int<T>::is_signed ? 1 : 0);
      constexpr static int digits10 =
          static_cast<int>(numeric_limits_impl_int<T>::digits * 0.30102999566);
      constexpr static int max_digits10 = 0;
      constexpr static int radix = 2;

      constexpr static int min_exponent = 0;
      constexpr static int min_exponent10 = 0;
      constexpr static int max_exponent = 0;
      constexpr static int max_exponent10 = 0;
      constexpr static bool traps = false;
      constexpr static bool tinyness_before = false;

      static constexpr T min() { return is_signed ? (~T()) ^ (~T() >> 1) : T(); }
      static constexpr T lowest() { return min(); }
      static constexpr T max() {
        if (xtd::is_integral<T>::value) {
          if (xtd::is_signed<T>::value) {
	    return (T(-1) >> 1);
          } else {
	    return T(-1);
          }
        } else {
	  return T(0);
        }
      }
      static constexpr T epsilon() { return 0; }
      static constexpr T round_error() { return 0; }
      static constexpr T infinity() { return 0; }
      static constexpr T quiet_NaN() { return 0; }
      static constexpr T signaling_NaN() { return 0; }
      static constexpr T denorm_min() { return 0; }
    };
  }  // namespace detail

  template <typename T>
  class numeric_limits {
  public:
    constexpr static bool is_specialized = false;
    constexpr static bool is_signed = false;
    constexpr static bool is_integer = false;
    constexpr static bool is_exact = false;
    constexpr static bool has_infinity = false;
    constexpr static bool has_quiet_NaN = false;
    constexpr static bool has_signaling_NaN = false;
    constexpr static xtd::float_denorm_style has_denorm = xtd::float_denorm_style::denorm_absent;
    constexpr static bool has_denorm_loss = false;
    constexpr static xtd::float_round_style round_style = xtd::round_toward_zero;
    constexpr static bool is_iec559 = false;
    constexpr static bool is_bounded = false;
    constexpr static bool is_modulo = false;
    constexpr static int digits = 0;
    constexpr static int digits10 = 0;
    constexpr static int max_digits10 = 0;
    constexpr static int radix = 0;

    constexpr static int min_exponent = 0;
    constexpr static int min_exponent10 = 0;
    constexpr static int max_exponent = 0;
    constexpr static int max_exponent10 = 0;
    constexpr static bool traps = false;
    constexpr static bool tinyness_before = false;

    static constexpr T min() { return T(); }
    static constexpr T lowest() { return T(); }
    static constexpr T max() { return T(); }
    static constexpr T epsilon() { return T(); }
    static constexpr T round_error() { return T(); }
    static constexpr T infinity() { return T(); }
    static constexpr T quiet_NaN() { return T(); }
    static constexpr T signaling_NaN() { return T(); }
    static constexpr T denorm_min() { return T(); }
  };

  template <>
  class numeric_limits<char> : public detail::numeric_limits_impl_int<char> {};
  template <>
  class numeric_limits<signed char> : public detail::numeric_limits_impl_int<signed char> {};
  template <>
  class numeric_limits<unsigned char> : public detail::numeric_limits_impl_int<unsigned char> {};

  template <>
  class numeric_limits<short> : public detail::numeric_limits_impl_int<short> {};
  template <>
  class numeric_limits<unsigned short> : public detail::numeric_limits_impl_int<unsigned short> {};
  template <>
  class numeric_limits<int> : public detail::numeric_limits_impl_int<int> {};
  template <>
  class numeric_limits<unsigned int> : public detail::numeric_limits_impl_int<unsigned int> {};
  template <>
  class numeric_limits<long> : public detail::numeric_limits_impl_int<long> {};
  template <>
  class numeric_limits<unsigned long> : public detail::numeric_limits_impl_int<unsigned long> {};
  template <>
  class numeric_limits<long long> : public detail::numeric_limits_impl_int<long long> {};
  template <>
  class numeric_limits<unsigned long long>
      : public detail::numeric_limits_impl_int<unsigned long long> {};

  /*
TBD
template <>
class numeric_limits<wchar_t>;
template <>
class numeric_limits<bool>;
template <>
class numeric_limits<float>;
template <>
class numeric_limits<double>;
template <>
class numeric_limits<long double>;
  */
}  // namespace xtd

#endif
