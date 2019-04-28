#include "xtd_uc/common.hpp"

#include "xtd_uc/adc.hpp"
#include "xtd_uc/bootstrap.hpp"
#include "xtd_uc/wdt.hpp"

namespace xtd {
  reset_cause bootstrap(bool enable_wdt) {
    reset_cause rst_cause = reset_cause::power_on;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
      wdt_reset_timeout();
      rst_cause = static_cast<reset_cause>(MCUSR);

      // If MCUSR has WDRF set, it must be cleared before changing WDE flag.
      if (enable_wdt) {
        wdt_enable(wdt_timeout::_16ms, false, true);
        MCUSR = 0;
      } else {
        MCUSR = 0;
        wdt_disable();
      }

      adc_disable();

      // Enable all power reduction
      PRR = 0xff;
      ACSR = _BV(ACD);  // Disable analog comparator

      wdt_reset_timeout();
    }
    return rst_cause;
  }
}  // namespace xtd
