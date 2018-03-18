#ifndef XTD_UC_DELAY_HPP
#define XTD_UC_DELAY_HPP
#include "common.hpp"

#include "chrono_noclock.hpp"

namespace xtd {
  using delay_duration = xtd::chrono::duration<long, xtd::ratio<4, F_CPU>>;

  // Delays the MCU by the specified duration.
  //
  // You should prefer to use `sleep()` instead of `delay()` where possible.
  //
  // Cases where delay() should be used over sleep() include:
  //      * The precision of sleep isn't good enough.
  //      * xtd::chrono::steady_clock isn't used or activated.
  //      * You do not want the MCU to enter any power saving mode (IDLE/POWERSAVE).
  //
  // The largest delay possible on a 16MHz clock is 536 seconds or slightly more than 9 minutes.
  // The largest possible delay is inversely proportional to the clock speed. So 8MHz has 18 minutes
  // maximum delay.
  //
  // The accuracy of the delay is determined by the accuracy of the system clock and the overhead to
  // setup the delay counters.
  // The precison of the delay is 4/F_CPU seconds. For a 16MHz clock, this is 250 ns. 
  void delay(const delay_duration& d);
}

#endif
