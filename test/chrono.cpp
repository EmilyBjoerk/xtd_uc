#include "xtd_uc/chrono.hpp"
#include <chrono>
#include <gtest/gtest.h>

using namespace std::chrono_literals;
using namespace xtd::unit_literals;

using xtd_duration = xtd::chrono::steady_clock::duration;
using clock_period = xtd_duration::scale;
using std_duration =
    std::chrono::duration<long long, std::ratio<clock_period::num, clock_period::den>>;

template <typename Rep, typename Period>
constexpr xtd::chrono::steady_clock::duration std2xtdchrono(std::chrono::duration<Rep, Period> x) {
  return xtd_duration{std::chrono::duration_cast<std_duration>(x).count()};
}

TEST(Chrono, ConversionTest) { ASSERT_EQ(1_s, std2xtdchrono(std::chrono::milliseconds(1000))); }
