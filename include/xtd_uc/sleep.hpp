#ifndef XTD_UC_SLEEP_HPP
#define XTD_UC_SLEEP_HPP
#include "common.hpp"

#include <avr/sleep.h>
#include "chrono.hpp"
#include "delay.hpp"

namespace xtd {

  // Defines a callback type for optional processing when the MCU is woken from sleep by an IRQ.
  using irq_wake_callback = bool (*)();

  // Sleeps the MCU for the desired time.
  //
  // CAUTION: Global Interrputs must be enabled before calling sleep!
  // 
  // Largest sleep possible is dictated by xtd::chrono::steady_clock::duration.
  //
  // The precision of the sleep is dictated by the precision of xtd::chrono::steady_clock,
  // for a 16MHz MCU this is 64µs. If you need higher precision than what sleep can provide on your
  // MCU, you need to use `delay()`.
  //
  // Calling sleep will enter a low power mode. The sleep command is not as accurate as delay()
  // since delay is a timed busy wait and sleep() relieas on the xtd::chrono::steady_clock which
  // isn't wall time and may be affected by other long running interrupt service routines or long
  // regions where interrupts are disabled.
  //
  // If "deep==false", then only the cpu clock and flash clock will be halted. All I/O peripheral
  // clocks and interrupts will still be processed. (IDLE mode in AVR data sheets)
  // If "deep==true" a deeper sleep state will be entered where only external interrupts, TWI and
  // Watchdog will wake the device. Other I/O such as USART will not process (Power-save mode in
  // AVR data sheets).
  //
  // ISRs will be serviced in accordance to the deep mode flag but the call will not return until
  // the full duration has been slept. If the `irq_wake` parameter is not null then the function
  // pointed to will be called when the MCU wakes from an IRQ and before it goes back to sleep
  // again. If the `irq_wake` function returns false, then the sleep function returns prematurely.
  // Note that `irq_wake` will be called everytime TIMER/COUNTER2 overflows, i.e. every
  // std::chrono::steady_clock::irq_period seconds. So it's a suitable place to reset the watchdog
  // timer for example to avoid the sleep causing the watchdog to reset the MCU.
  void sleep(const xtd::chrono::steady_clock::duration& d, bool deep = false,
             irq_wake_callback irq_wake = nullptr);
}

#endif
