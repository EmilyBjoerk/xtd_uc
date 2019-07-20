#include "xtd_uc/common.hpp"

#include "xtd_uc/sleep.hpp"
#include "xtd_uc/wdt.hpp"

using namespace xtd::unit_literals;

namespace xtd {
  using chrono::duration;
  using chrono::steady_clock;

  // This could have been a template function with a custom duration. However by
  // making it a free function we avoid multiple instantiations with the same
  // (rather large) code which saves on flash space. Further more we know that
  // the steady_clock::duration and irq_period are related by a power of two
  // which simplifies the generated code further as multiplications can be avoided.
  //
  // The conversion from other durations to the steady_clock duration is done automatically
  // and is done at the call site. Sleeping constant amounts of time enjoys compile time
  // constant expansion to remove the conversion overhead.
  void sleep(const steady_clock::duration& d, bool deep, irq_wake_callback irq_wake) {
    constexpr auto irq_duration =
        steady_clock::duration(duration<steady_clock::value_type, steady_clock::irq_period>(1));
    const auto end = steady_clock::now() + d;
    const auto end_safe = end - irq_duration;

    // Both Idle and Power-save modes will wake the device on Timer/Counter2 interrupt.
    // Calling xtd::steady_clock::now() above will enable Timer/Counter2.
    // Thus the device will wake from the sleep every "steady_clock::irq_period" seconds.
    //
    // Power-save mode is selected by passing "deep=true".
    sleep_enable();
    set_sleep_mode(deep ? SLEEP_MODE_PWR_SAVE : SLEEP_MODE_IDLE);
    while (steady_clock::now() < end_safe) {
      xtd::wdt_reset_timeout();
      if (irq_wake && !irq_wake()) {
        sleep_disable();
        return;
      }

      // Interrupts may make each sleep shorter than the irq_period.
      // Because of this dead counting wount work.
      sleep_cpu();
    }
    sleep_disable();

    // Busy wait the remainder
    delay(end - steady_clock::now());
  }
}  // namespace xtd
