#include "xtd_uc/utility.hpp"
#include <gtest/gtest.h>

#include "xtd_uc/cstdint.hpp"

TEST(Swap, Compute) {
  int a = 3;
  int b = 1;
  xtd::swap(a, b);
  ASSERT_EQ(1, a);
  ASSERT_EQ(3, b);
}
