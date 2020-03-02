#include "xtd_uc/common.hpp"

#include "xtd_uc/adc.hpp"
#include "xtd_uc/bootstrap.hpp"
#include "xtd_uc/wdt.hpp"

namespace xtd {
  reset_cause bootstrap(bool enable_wdt, wdt_timeout timeout) {
    auto rst_cause = reset_cause::power_on;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
      wdt_reset_timeout();
      rst_cause = static_cast<reset_cause>(MCUSR);

      // If MCUSR has WDRF set, it must be cleared before changing WDE flag.
      if (enable_wdt) {
        wdt_enable(timeout, false, true);
        MCUSR = 0;
      } else {
        MCUSR = 0;
        wdt_disable();
      }

      PRR = 0xFF;       // Enable all power reduction
      ACSR = _BV(ACD);  // Disable analog comparator (and clear irq flag)

      wdt_reset_timeout();
    }
    return rst_cause;
  }
}  // namespace xtd
