#include "xtd_uc/units.hpp"
#include <gtest/gtest.h>

#include <iostream>

using namespace xtd;
using namespace xtd::unit_literals;

TEST(Quantity, SelfEquality) { ASSERT_EQ(1_m, 1_m); }

TEST(Quantity, ScaleEq) {
  ASSERT_EQ(1_F, 1000_mF);
  ASSERT_EQ(1000_uF, 1_mF);
}

TEST(Quantity, ScaleNotEq) {
  ASSERT_NE(1_F, 1000_uF);
  ASSERT_NE(1000_uF, 1_F);
}

TEST(Quantity, ScaleLess) { ASSERT_LT(1_mF, 1_F); }

TEST(Quantity, ScaleGreater) { ASSERT_GT(1_mF, 1_pF); }

TEST(Quantity, ScaleLessEquals) {
  ASSERT_LE(1_F, 1000_mF);
  ASSERT_LE(999_mF, 1_F);
}

TEST(Quantity, ScaleGreaterEquals) {
  ASSERT_GE(1_F, 1000_mF);
  ASSERT_GE(1_F, 999_mF);
}

TEST(Quantity, UnitMult) {
  ASSERT_EQ(200_N, 20_kg * 10_m_s2);
  ASSERT_EQ(10000_mV, 500_mA * 20_Ohm);
  ASSERT_EQ(10_V, 500_mA * 20_Ohm);
}

TEST(Quantity, Conversion) {
  using two_mA = units::quantity<uint8_t, units::ampere, ratio<2,1000>>;
  two_mA cut = 500_mA;
  
  ASSERT_EQ(250, cut.v);
}
