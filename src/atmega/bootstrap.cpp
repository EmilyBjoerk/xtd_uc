#include "xtd_uc/common.hpp"

#include "xtd_uc/bootstrap.hpp"
#include "xtd_uc/adc.hpp"
#include "xtd_uc/wdt.hpp"

namespace xtd {
  reset_cause bootstrap(bool enable_wdt) {
    reset_cause rst_cause = reset_cause::power_on;
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
      watchdog::reset_timeout();
      rst_cause = static_cast<reset_cause>(MCUSR);

      if (enable_wdt) {
        WDTCSR |= _BV(WDCE);
        WDTCSR |= _BV(WDE);
      } else {
        WDTCSR |= _BV(WDCE);
        clr_bit(WDTCSR, WDE);
      }

      // The WDRF flag in MCUSR forces WDT on (so it remains on after a reset by WDT).
      // If we desire to not have WDT on, we must be sure to clear this flag. If we do
      // want WDT on, it will have been set earlier and we should clear this flag anyway.
      MCUSR = 0;

      // Enable all power reduction
      PRR = 0xff;

      // Disable digital inputs on adc and the adc
      adc::disable_digital();
      adc::disable();

      watchdog::reset_timeout();
    }
    return rst_cause;
  }
}
