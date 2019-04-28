#ifndef XTD_UC_BOOTSTRAP_HPP
#define XTD_UC_BOOTSTRAP_HPP
#include "common.hpp"

#include <stdint.h>

namespace xtd {
  enum class reset_cause : uint8_t { watchdog = 8, brownout = 4, external = 2, power_on = 1 };

  /*
   * Performs initial bootstrap of the AVR MCU. Should be executed as soon as possible.
   * It makes sure that the MCU will not be suddenly reset by Watchdog Timer and sets the
   * state of the Watchdog Timer to the desired reset value. It does so in a manner that
   * doesn't go through a state of WDT disabled if it was previously enabled.
   *
   * If the WDT reset is enabled, you must configure the WDT timeout by calling
   * watchdog::set_timeout() immediately after calling bootstrap.
   *
   * Bootstrap will enable power reduction by disabling all optional chip features.
   * You will need to individually enable the features you need. This has the implication that
   * if you use xtd::chrono::steady_clock() it will start counting time from the first call to
   * `now()` when it enables the power to the timer circuit. So it's not time from poweron, but
   * time from first call.
   */
  reset_cause bootstrap(bool enable_wdt_reset);
}  // namespace xtd

#endif
