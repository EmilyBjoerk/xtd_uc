#include "xtd_uc/cmath.hpp"
#include <gtest/gtest.h>

TEST(MathDivide, RoundingNearest) {
  ASSERT_EQ(1, (xtd::divide<int, xtd::round_style::nearest>(1, 2)));
  ASSERT_EQ(-1, (xtd::divide<int, xtd::round_style::nearest>(1, -2)));
  ASSERT_EQ(-1, (xtd::divide<int, xtd::round_style::nearest>(-1, 2)));
  ASSERT_EQ(1, (xtd::divide<int, xtd::round_style::nearest>(-1, -2)));

  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::nearest>(1, 3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::nearest>(1, -3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::nearest>(-1, 3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::nearest>(-1, -3)));

  ASSERT_EQ(1, (xtd::divide<int, xtd::round_style::nearest>(4, 3)));
  ASSERT_EQ(-1, (xtd::divide<int, xtd::round_style::nearest>(4, -3)));
  ASSERT_EQ(-1, (xtd::divide<int, xtd::round_style::nearest>(-4, 3)));
  ASSERT_EQ(1, (xtd::divide<int, xtd::round_style::nearest>(-4, -3)));
}

TEST(MathDivide, RoundingTruncate) {
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::truncate>(1, 3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::truncate>(-1, 3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::truncate>(1, -3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::truncate>(-1, -3)));

  ASSERT_EQ(1, (xtd::divide<int, xtd::round_style::truncate>(4, 3)));
  ASSERT_EQ(-1, (xtd::divide<int, xtd::round_style::truncate>(-4, 3)));
  ASSERT_EQ(-1, (xtd::divide<int, xtd::round_style::truncate>(4, -3)));
  ASSERT_EQ(1, (xtd::divide<int, xtd::round_style::truncate>(-4, -3)));

  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::truncate>(2, 3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::truncate>(-2, 3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::truncate>(2, -3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::truncate>(-2, -3)));
}

TEST(MathDivide, RoundingFloor) {
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::floor>(1, 2)));
  ASSERT_EQ(-1, (xtd::divide<int, xtd::round_style::floor>(-1, 2)));
  ASSERT_EQ(-1, (xtd::divide<int, xtd::round_style::floor>(1, -2)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::floor>(-1, -2)));

  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::floor>(1, 3)));
  ASSERT_EQ(-1, (xtd::divide<int, xtd::round_style::floor>(-1, 3)));
  ASSERT_EQ(-1, (xtd::divide<int, xtd::round_style::floor>(1, -3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::floor>(-1, -3)));
}

TEST(MathDivide, RoundingCeil) {
  ASSERT_EQ(1, (xtd::divide<int, xtd::round_style::ceil>(1, 3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::ceil>(-1, 3)));
  ASSERT_EQ(0, (xtd::divide<int, xtd::round_style::ceil>(1, -3)));
  ASSERT_EQ(1, (xtd::divide<int, xtd::round_style::ceil>(-1, -3)));
}
