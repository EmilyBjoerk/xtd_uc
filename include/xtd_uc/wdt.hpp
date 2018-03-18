#ifndef XTD_UC_WDT_HPP
#define XTD_UC_WDT_HPP
#include "common.hpp"

#include <avr/wdt.h>
#include <stdint.h>
#include <util/atomic.h>

#include "utility.hpp"

namespace xtd {
  namespace watchdog {
    enum class timeout : uint8_t {
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

    inline void reset_timeout() { wdt_reset(); }
    inline void set_timeout(timeout to) {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        reset_timeout();
        set_bit(WDTCSR, WDCE);
        WDTCSR |= static_cast<uint8_t>(to);
      }
    }

    inline void enable_irq() { set_bit(WDTCSR, WDIE); }
    inline void disable_irq() { clr_bit(WDTCSR, WDIE); }

    inline bool reset_enabled() { return WDTCSR & _BV(WDE); }

    inline void enable_reset() {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        reset_timeout();
        WDTCSR |= _BV(WDCE);
        WDTCSR |= _BV(WDE);
      }
    }

    inline void disable_reset() {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        reset_timeout();
        WDTCSR |= _BV(WDCE);
        clr_bit(WDTCSR, WDE);
      }
    }
  }
}
#endif
