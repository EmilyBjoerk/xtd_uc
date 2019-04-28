#include "xtd_uc/common.hpp"

#include "xtd_uc/blink.hpp"
#include "xtd_uc/gpio.hpp"

namespace xtd {
  void blink(gpio_pin pin, uint8_t times, const xtd::delay_duration& d) {
    if (times == 0) {
      while (1) {
        gpio_write(pin, true);
        delay(d);
        gpio_write(pin, false);
        delay(d);
      }
    } else {
      for (uint8_t i = 0; i < times; ++i) {
        gpio_write(pin, true);
        delay(d);
        gpio_write(pin, false);
        delay(d);
      }
    }
  }
}  // namespace xtd
