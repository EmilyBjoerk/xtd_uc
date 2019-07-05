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

TEST(Quantity, Multiplication) {
  ASSERT_EQ(200_N, 20_kg * 10_m_s2);
  ASSERT_EQ(10000_mV, 500_mA * 20_Ohm);
  ASSERT_EQ(10_V, 500_mA * 20_Ohm);
}

TEST(Quantity, ScaleOverflowBug1) { ASSERT_EQ(1999_pF, 2_nF - 1_pF); }

TEST(Quantity, Inversion) { ASSERT_EQ(10_Hz, 1 / 100_ms); }

TEST(Quantity, Division) { ASSERT_EQ(25_uA, 500_mV / 20_kOhm); }

TEST(Quantity, Additions) { ASSERT_EQ(1001_g, 1_kg + 1_g); }

TEST(Quantity, Additions_ScaleOverflow) {
  using capacitance =
      units::capacitance<uint32_t, ratio_t<999999999999999999, 1000000000000000000>>;

  ASSERT_EQ(capacitance(2), capacitance(1) + capacitance(1));
}

TEST(Quantity, Subtraction) { ASSERT_EQ(999_g, 1_kg - 1_g); }

TEST(Quantity, FreqConversion) {
  auto f = units::frequency<uint32_t, ratio<1>>(100_kHz);
  ASSERT_EQ(100000, f.count());
}

TEST(Quantity, Conversion) {
  using two_mA = units::current<uint8_t, ratio<2, 1000>>;
  two_mA cut = 500_mA;

  ASSERT_EQ(250, cut.count());
}

TEST(Quantity, MakeUnity) {
  constexpr auto volts = 123_mV;
  auto unity_volts = units::make_unity_valued<volts.count()>(volts);

  ASSERT_EQ(1, unity_volts.count());
  static_assert(xtd::is_same_v<units::volt, decltype(unity_volts)::units>,
                "Wrong unit for make unity");
  ASSERT_EQ(volts, unity_volts);
}
