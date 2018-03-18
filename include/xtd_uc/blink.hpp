#ifndef XTD_UC_BLINK_HPP
#define XTD_UC_BLINK_HPP
#include "common.hpp"

#include "gpio.hpp"
#include "delay.hpp"

namespace xtd {
  // Blinks the led 'times' times. If times is 0, then blink indefinitely (never returns).
  void blink(gpio_pin pin, uint8_t times, const xtd::delay_duration& d);
}

#endif
