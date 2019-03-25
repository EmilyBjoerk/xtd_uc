#include "xtd_uc/delay.hpp"
#include "xtd_uc/common.hpp"

#ifdef ENABLE_TEST
#include <chrono>
#include <thread>
#else
#include <util/delay.h>  // _delay_loop_2
#endif

namespace xtd {

  // This could have been implemented as a template function with a custom duration.
  // However by making it a free plain function we avoid multiple instantiations with different
  // types which saves code space. We also use the period which is internally used which
  // pushes the conversion to the correct duration type to the caller. Which means that the caller
  // is able to choose if they want to reduce code size by matching the duration type.
  // Further more the conversion is done at the call site and if the argument is a constant
  // expression the conversion enjoys compile time constant propagation and is free of overhead.
  void delay(const delay_duration& d) {
#ifdef ENABLE_TEST
    std::this_thread::sleep_for(std::chrono::nanoseconds(xtd::chrono::nanoseconds(d).count()));

#else
    auto counts = d.count();
    static_assert(sizeof(counts) >= 4, "counts way too small");

    if (counts <= 0) return;

    constexpr int32_t max_delay = 65536;
    // This is the amount we subtract from our sleep counter. A sleep value of 0 will
    // sleep for 4*65536 cycles. We compensate for the fact that we do some other
    // 64 bit arithmetic outside th delay loop.
    constexpr int32_t max_delay_comp = 65536 + 4;

    while (counts >= max_delay) {
      _delay_loop_2(0);  // 0 sleeps 2^16 * 4 cycles.
      counts -= max_delay_comp;
    }

    if (counts > 0) {
      _delay_loop_2(static_cast<uint16_t>(counts));
    }
#endif
  }
}  // namespace xtd
