#ifndef XTD_UC_BLINK_HPP
#define XTD_UC_BLINK_HPP
#include "common.hpp"
#include "delay.hpp"
#include "gpio2.hpp"

namespace xtd {
  template <typename pin>
  void blink(uint8_t times, const xtd::delay_duration& d) {
    pin::output(true);
    while (times-- > 0) {
      pin::toggle();
      delay(d);
    }
  }

  template <typename pin>
  void blink(const xtd::delay_duration& d) {
    pin::output(true);
    while (true) {
      pin::toggle();
      delay(d);
    }
  }
}  // namespace xtd

#endif
