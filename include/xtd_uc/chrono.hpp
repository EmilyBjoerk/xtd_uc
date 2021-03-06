#ifndef XTD_UC_CHRONO_HPP
#define XTD_UC_CHRONO_HPP
#include "common.hpp"

#include "chrono_noclock.hpp"

// -----------------------------------------------------------------------------
//
// NOTE: Including "chrono.hpp" will reserve TIMER2 for use by the steady_clock.
//
// To use "chrono.hpp" you must build and link "chrono.cpp"
//
// -----------------------------------------------------------------------------

#ifndef ENABLE_TEST
#include <avr/interrupt.h>

// To allow friend declaration in steady_clock
extern "C" void TIMER2_OVF_vect(void) __attribute__((signal));
#endif

namespace xtd {
  namespace chrono {
    // Implements a steady clock.
    //
    // The steady_clock overflows after 18718157 years on a 16MHz clock speed.
    // The steady_clock time is guaranteed to always increase, up until the overflow point.
    // The time starts the first time `now()` is called.
    //
    // Implementation:
    //     The steady_clock is implemented using the TIMER/COUNTER2 on the MCU driven by interrupts.
    //     We choose TIMER/COUNTER2 because as opposed to timer 0 or 1 TIMER/COUNTER2 stays enabled
    //     during SLEEP_MODE_POWERSAVE and allows time to pass when the MCU is in power saving
    //     sleep.
    //
    // Clock precision:
    //     The clock precision is 1024 / F_CPU seconds. For a 16MHz clock this is 64µs.
    //
    // Clock accuracy:
    //     The accuracy of the steady_clock is dicated by the accuracy of the system clock speed and
    //     the other interrupt handlers in the system as well as the usage of MCU sleep modes.
    //
    //     To guarantee the accuracy of the clock the following conditions must be fullfilled:
    //         * All ISRs must complete in less than 1024*256 cycles.
    //         * No other sleep mode than SLEEP_MODE_IDLE or SLEEP_MODE_POWERSAVE may be used.
    //         * Global interrupts must be enabled as default and interrupts may not be disabled
    //           for more than or equal to 256*1024 cycles.
    //
    //     Despite the above the clock may lag behind if the TIMER/COUNTER2 ISR is prevented from
    //     running by other reasons. Like for extremely many ISRs being executed in combination
    //     with code blocks where interrupts are disabled. To minimize this chance, keep ISRs
    //     short and few, and also keep code blocks with interrupts disabled to a minimum length.
    class steady_clock {
    public:
      using value_type = long long;
      using scale = ratio_t<1024UL, F_CPU>;  // For 16MHz -> 1 : 15625
      using duration = xtd::chrono::duration<value_type, scale>;
      using time_point = xtd::chrono::time_point<steady_clock, duration>;
      using irq_period = ratio_multiply<scale, ratio<256>>;

      static_assert(decltype(time_point() + duration())::scale::den == duration::scale::den,
                    "period mismatch");
      static_assert(decltype(time_point() + duration())::scale::num == duration::scale::num,
                    "num mismatch");

      constexpr static bool is_steady = true;
      static time_point now();

    private:
      steady_clock();
#ifndef ENABLE_TEST
      friend void ::TIMER2_OVF_vect(void);
#endif
      static volatile value_type ticks;
    };
  }  // namespace chrono
}  // namespace xtd

#endif
