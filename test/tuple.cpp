#include "xtd_uc/tuple.hpp"
#include <gtest/gtest.h>

TEST(Tuple, MakeAndGetAllMembers) {
  auto cut = xtd::make_tuple(1, 2.3, 'a');
  const auto ccut = xtd::make_tuple(1, 2.3, 'a');

  static_assert(std::is_same<decltype(xtd::get<0>(cut)), int&>::value,
                "Wrong type for first value!");
  static_assert(std::is_same<decltype(xtd::get<1>(cut)), double&>::value,
                "Wrong type for second value!");
  static_assert(std::is_same<decltype(xtd::get<2>(cut)), char&>::value,
                "Wrong type for third value!");

  static_assert(std::is_same<decltype(xtd::get<0>(ccut)), const int&>::value,
                "Wrong type for first value!");
  static_assert(std::is_same<decltype(xtd::get<1>(ccut)), const double&>::value,
                "Wrong type for second value!");
  static_assert(std::is_same<decltype(xtd::get<2>(ccut)), const char&>::value,
                "Wrong type for third value!");

  static_assert(3 == decltype(cut)::size(), "Wrong number of elements!");

  ASSERT_EQ(1, xtd::get<0>(cut));
  ASSERT_EQ(2.3, xtd::get<1>(cut));
  ASSERT_EQ('a', xtd::get<2>(cut));

  ASSERT_EQ(1, xtd::get<0>(ccut));
  ASSERT_EQ(2.3, xtd::get<1>(ccut));
  ASSERT_EQ('a', xtd::get<2>(ccut));
}

TEST(Tuple, MakeArgumentsDecay) {
  struct expected_struct {
    int x;
    const char* y;
  };

  int x = 3;
  const int& xref = x;

  auto cut = xtd::make_tuple(xref, "foo");

  static_assert(std::is_same<decltype(xtd::get<0>(cut)), int&>::value,
                "Wrong type for first value!");

  static_assert(sizeof(expected_struct) == sizeof(cut), "Wrong size!");

  const char* str = xtd::get<1>(cut);

  ASSERT_EQ(3, xtd::get<0>(cut));

  x = 5;
  ASSERT_EQ(3, xtd::get<0>(cut));
  ASSERT_EQ("foo", str);
}
