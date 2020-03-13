#include "xtd_uc/limits.hpp"

#include <limits>
#include "xtd_uc/type_traits.hpp"

template <typename T>
using cut = xtd::numeric_limits<T>;

template <typename T>
using ref = std::numeric_limits<T>;

constexpr bool operator==(const xtd::float_denorm_style x, const std::float_denorm_style s) {
  return (x == xtd::denorm_indeterminate && s == std::denorm_indeterminate) ||
         (x == xtd::denorm_absent && s == std::denorm_absent) ||
         (x == xtd::denorm_present && s == std::denorm_present);
}

constexpr bool operator==(const xtd::float_round_style x, const std::float_round_style s) {
  return (x == xtd::round_indeterminate && s == std::round_indeterminate) ||
         (x == xtd::round_toward_zero && s == std::round_toward_zero) ||
         (x == xtd::round_to_nearest && s == std::round_to_nearest) ||
         (x == xtd::round_toward_infinity && s == std::round_toward_infinity) ||
         (x == xtd::round_toward_neg_infinity && s == std::round_toward_neg_infinity);
}

template <typename T>
struct verify_equality {
  static_assert(cut<T>::is_specialized == ref<T>::is_specialized, "is_specialized");
  static_assert(cut<T>::is_signed == ref<T>::is_signed, "is_signed");
  static_assert(cut<T>::is_integer == ref<T>::is_integer, "is_integer");
  static_assert(cut<T>::is_exact == ref<T>::is_exact, "is_exact");

  static_assert(cut<T>::has_infinity == ref<T>::has_infinity, "has_infinity");
  static_assert(cut<T>::has_quiet_NaN == ref<T>::has_quiet_NaN, "has_quiet_NaN");
  static_assert(cut<T>::has_signaling_NaN == ref<T>::has_signaling_NaN, "has_signaling_NaN");
  static_assert(cut<T>::has_denorm == ref<T>::has_denorm, "has_denorm");
  static_assert(cut<T>::has_denorm_loss == ref<T>::has_denorm_loss, "has_denorm_loss");
  static_assert(cut<T>::round_style == ref<T>::round_style, "round_style");
  static_assert(cut<T>::is_iec559 == ref<T>::is_iec559, "is_iec559");
  static_assert(cut<T>::is_bounded == ref<T>::is_bounded, "is_bounded");
  static_assert(cut<T>::is_modulo == ref<T>::is_modulo, "is_modulo");
  static_assert(cut<T>::digits == ref<T>::digits, "digits");
  static_assert(cut<T>::digits10 == ref<T>::digits10, "digits10");
  static_assert(cut<T>::max_digits10 == ref<T>::max_digits10, "max_digits10");
  static_assert(cut<T>::radix == ref<T>::radix, "radix");

  static_assert(cut<T>::min_exponent == ref<T>::min_exponent, "min_exponent");
  static_assert(cut<T>::min_exponent10 == ref<T>::min_exponent10, "min_exponent10");
  static_assert(cut<T>::max_exponent == ref<T>::max_exponent, "max_exponent");
  static_assert(cut<T>::max_exponent10 == ref<T>::max_exponent10, "max_exponent10");
  // We don't support this yet
  // static_assert(cut<T>::traps == ref<T>::traps, "traps");
  static_assert(cut<T>::tinyness_before == ref<T>::tinyness_before, "tinyness_before");

  static_assert(cut<T>::min() == ref<T>::min(), "min()");
  static_assert(cut<T>::max() == ref<T>::max(), "max()");
  static_assert(cut<T>::lowest() == ref<T>::lowest(), "lowest()");
  static_assert(cut<T>::epsilon() == ref<T>::epsilon(), "epsilon()");
  static_assert(cut<T>::round_error() == ref<T>::round_error(), "round_error()");
  static_assert(cut<T>::infinity() == ref<T>::infinity(), "infinity()");
  static_assert(cut<T>::quiet_NaN() == ref<T>::quiet_NaN(), "quiet_NaN()");
  static_assert(cut<T>::signaling_NaN() == ref<T>::signaling_NaN(), "signaling_NaN()");
  static_assert(cut<T>::denorm_min() == ref<T>::denorm_min(), "denorm_min()");

  constexpr static bool test = true;
};

static_assert(ref<char>::is_signed == true, "");
static_assert(ref<signed char>::is_signed == true, "");
static_assert(ref<unsigned char>::is_signed == false, "");

// Not supported 
static_assert(verify_equality<bool>::test == true, "");
static_assert(verify_equality<char>::test == true, "");
static_assert(verify_equality<unsigned char>::test == true, "");
static_assert(verify_equality<short>::test == true, "");
static_assert(verify_equality<unsigned short>::test == true, "");
static_assert(verify_equality<int>::test == true, "");
static_assert(verify_equality<unsigned int>::test == true, "");
static_assert(verify_equality<long>::test == true, "");
static_assert(verify_equality<unsigned long>::test == true, "");
static_assert(verify_equality<long long>::test == true, "");
static_assert(verify_equality<unsigned long long>::test == true, "");

static_assert(verify_equality<int8_t>::test == true, "");
static_assert(verify_equality<uint8_t>::test == true, "");
static_assert(verify_equality<int16_t>::test == true, "");
static_assert(verify_equality<uint16_t>::test == true, "");
static_assert(verify_equality<int32_t>::test == true, "");
static_assert(verify_equality<uint32_t>::test == true, "");
static_assert(verify_equality<int64_t>::test == true, "");
static_assert(verify_equality<uint64_t>::test == true, "");

