#ifndef XTD_UC_BOOTSTRAP_HPP
#define XTD_UC_BOOTSTRAP_HPP
#include "common.hpp"

#include "wdt.hpp"
#include <stdint.h>

namespace xtd {
  enum class reset_cause : uint8_t { watchdog = 8, brownout = 4, external = 2, power_on = 1 };

  /** 
   * Brings the AVR to a more sensible default start up state by powering down 
   * optional chip features to save power when they are not used. Will read and
   * return the reset cause and setup WDT to the desired state (without going 
   * through a disabled state if enabled was requested).
   *
   * Call as soon as possible after reset. 
   *
   * Note that by calling bootstrap(), chrono::steady_clock::now() counts time 
   * from first call to now() and not power on.
   */
  reset_cause bootstrap(bool enable_wdt_reset, wdt_timeout timeout = wdt_timeout::_16ms);
}  // namespace xtd

#endif
