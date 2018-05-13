#include "xtd_uc/ratio.hpp"
#include <gtest/gtest.h>

TEST(RatioScale, RoundingNearest) {
  ASSERT_EQ(1, (xtd::ratio_scale<xtd::ratio<1, 2>, int, xtd::round_style::nearest>(1)));
  ASSERT_EQ(1, (xtd::ratio_scale<xtd::ratio<-1, 2>, int, xtd::round_style::nearest>(-1)));
  ASSERT_EQ(-1, (xtd::ratio_scale<xtd::ratio<-1, 2>, int, xtd::round_style::nearest>(1)));
  ASSERT_EQ(-1, (xtd::ratio_scale<xtd::ratio<1, 2>, int, xtd::round_style::nearest>(-1)));

  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<1, 3>, int, xtd::round_style::nearest>(1)));
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<-1, 3>, int, xtd::round_style::nearest>(-1)));
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<-1, 3>, int, xtd::round_style::nearest>(1)));
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<1, 3>, int, xtd::round_style::nearest>(-1)));
}

TEST(RatioScale, RoundingTruncate) {
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<1, 3>, int, xtd::round_style::truncate>(1)));
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<-1, 3>, int, xtd::round_style::truncate>(-1)));
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<-1, 3>, int, xtd::round_style::truncate>(1)));
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<1, 3>, int, xtd::round_style::truncate>(-1)));
}

TEST(RatioScale, RoundingFloor) {
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<1, 2>, int, xtd::round_style::floor>(1)));
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<-1, 2>, int, xtd::round_style::floor>(-1)));
  ASSERT_EQ(-1, (xtd::ratio_scale<xtd::ratio<-1, 2>, int, xtd::round_style::floor>(1)));
  ASSERT_EQ(-1, (xtd::ratio_scale<xtd::ratio<1, 2>, int, xtd::round_style::floor>(-1)));

  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<1, 3>, int, xtd::round_style::floor>(1)));
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<-1, 3>, int, xtd::round_style::floor>(-1)));
  ASSERT_EQ(-1, (xtd::ratio_scale<xtd::ratio<-1, 3>, int, xtd::round_style::floor>(1)));
  ASSERT_EQ(-1, (xtd::ratio_scale<xtd::ratio<1, 3>, int, xtd::round_style::floor>(-1)));
}

TEST(RatioScale, RoundingCeil) {
  ASSERT_EQ(1, (xtd::ratio_scale<xtd::ratio<1, 3>, int, xtd::round_style::ceil>(1)));
  ASSERT_EQ(1, (xtd::ratio_scale<xtd::ratio<-1, 3>, int, xtd::round_style::ceil>(-1)));
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<-1, 3>, int, xtd::round_style::ceil>(1)));
  ASSERT_EQ(0, (xtd::ratio_scale<xtd::ratio<1, 3>, int, xtd::round_style::ceil>(-1)));
}
