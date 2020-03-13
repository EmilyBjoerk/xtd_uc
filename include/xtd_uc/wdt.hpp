#ifndef XTD_UC_WDT_HPP
#define XTD_UC_WDT_HPP
#include "common.hpp"

#ifndef ENABLE_TEST
#include <avr/wdt.h>
#include <util/atomic.h>
#endif

#include <stdint.h>

#include "utility.hpp"

namespace xtd {
  enum class wdt_timeout : uint8_t {
    _16ms = 0b000000,
    _32ms = 0b000001,
    _64ms = 0b000010,
    _125ms = 0b000011,
    _250ms = 0b000100,
    _500ms = 0b000101,
    _1000ms = 0b000110,
    _2000ms = 0b000111,
    _4000ms = 0b100000,
    _8000ms = 0b100001
  };

#ifdef ENABLE_TEST
  inline void wdt_disable() {}
  inline void wdt_disable_irq() {}
  inline void wdt_enable(wdt_timeout /*timeout*/, bool /*irq*/, bool /*reset*/) {}
  inline void wdt_enable_irq() {}
  inline bool wdt_reset_enabled() { return true; }
  inline void wdt_reset_timeout() {}

#else
  inline void wdt_disable() { wdt_enable(wdt_timeout::_16ms, false, false); }
  inline void wdt_disable_irq() { clr_bit(WDTCSR, WDIE); }
  inline void wdt_enable(wdt_timeout timeout, bool irq, bool reset) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      wdt_reset_timeout();
      uint8_t new_value = _BV(WDCE) | (irq ? _BV(WDIE) : 0) | (reset ? _BV(WDE) : 0) |
                          static_cast<uint8_t>(timeout);
      WDTCSR = new_value;
      WDTCSR = new_value;
    }
  }
  inline void wdt_enable_irq() { set_bit(WDTCSR, WDIE); }
  inline bool wdt_reset_enabled() { return WDTCSR & _BV(WDE); }
  inline void wdt_reset_timeout() { wdt_reset(); }
#endif

}  // namespace xtd

#endif
