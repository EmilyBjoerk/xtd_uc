#include "xtd_uc/adc.hpp"
#include "xtd_uc/blink.hpp"
#include "xtd_uc/uart.hpp"

#include <avr/interrupt.h>

constexpr auto c_led_pin = xtd::gpio_pin(xtd::port_b, 6);

using namespace xtd::unit_literals;

xtd::ostream<xtd::uart_stream_tag> uart;

int main() {
  sei();
  xtd::gpio_config(c_led_pin, xtd::output);
  xtd::uart_configure(nullptr);
  xtd::adc_enable(100_kHz, false, xtd::adc_internal_vcc, 0);
  xtd::adc_dio_pin(0, false);
  xtd::blink(c_led_pin, 5, 300_ms);
  uart << xtd::pstr(PSTR("start"));
  while (true) {
    xtd::blink(c_led_pin, 10, 40_ms);
    auto result = xtd::adc_read_single_low_noise();
    uart << result << "\r\n";
  }

  xtd::adc_disable();
}
