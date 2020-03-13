#include "xtd_uc/type_traits.hpp"
#include <type_traits>

// -----------------------------------------------------------------------------
// xtd::is_same (and xtd::is_same_v)
// -----------------------------------------------------------------------------
static_assert(xtd::is_same_v<char, char> == true, "");
static_assert(xtd::is_same_v<char, signed char> == false, "");
static_assert(xtd::is_same_v<char, unsigned char> == false, "");
static_assert(xtd::is_same_v<signed char, char> == false, "");
static_assert(xtd::is_same_v<signed char, signed char> == true, "");
static_assert(xtd::is_same_v<signed char, unsigned char> == false, "");
static_assert(xtd::is_same_v<unsigned char, char> == false, "");
static_assert(xtd::is_same_v<unsigned char, signed char> == false, "");
static_assert(xtd::is_same_v<unsigned char, unsigned char> == true, "");

static_assert(xtd::is_same_v<int, int> == true, "");
static_assert(xtd::is_same_v<int, signed int> == true, "");
static_assert(xtd::is_same_v<int, unsigned int> == false, "");
static_assert(xtd::is_same_v<signed int, int> == true, "");
static_assert(xtd::is_same_v<signed int, signed int> == true, "");
static_assert(xtd::is_same_v<signed int, unsigned int> == false, "");
static_assert(xtd::is_same_v<unsigned int, int> == false, "");
static_assert(xtd::is_same_v<unsigned int, signed int> == false, "");
static_assert(xtd::is_same_v<unsigned int, unsigned int> == true, "");

// -----------------------------------------------------------------------------
// xtd::enable_if (and xtd::enable_if_t) 
// -----------------------------------------------------------------------------
template <typename T, typename Enable = void>
struct test_enable_if {
  constexpr static bool success = false;
};

template <typename T>
struct test_enable_if<T, typename xtd::enable_if_t<std::is_same<T, char>::value>> {
  constexpr static bool success = true;
};

static_assert(test_enable_if<int>::success == false, "int is false");
static_assert(test_enable_if<char>::success == true, "char is false");
