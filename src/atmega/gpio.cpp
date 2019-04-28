#include "xtd_uc/common.hpp"

#include <avr/io.h>
#include "xtd_uc/gpio.hpp"
#include "xtd_uc/utility.hpp"

namespace xtd {
  constexpr auto& get_port(gpio_port port) {
    if (port == gpio_port::port_b) {
      return PORTB;
    } else if (port == gpio_port::port_c) {
      return PORTC;
    }
    return PORTD;
  }

  constexpr auto& get_ddr(gpio_port port) {
    if (port == gpio_port::port_b) {
      return DDRB;
    } else if (port == gpio_port::port_c) {
      return DDRC;
    }
    return DDRD;
  }

  constexpr auto& get_pin(gpio_port port) {
    if (port == gpio_port::port_b) {
      return PINB;
    } else if (port == gpio_port::port_c) {
      return PINC;
    }
    return PIND;
  }

  void gpio_config(gpio_pin pin, gpio_mode mode, bool value) {
    auto pin_nr = pin.pin();
    auto& io_port = get_port(pin.port());
    auto& io_ddr = get_ddr(pin.port());

    switch (mode) {
      case gpio_mode::tristate:
        clr_bit(io_ddr, pin_nr);
        clr_bit(io_port, pin_nr);
        break;
      case gpio_mode::pullup:
        set_bit(io_port, pin_nr);
        clr_bit(io_ddr, pin_nr);
        break;
      case gpio_mode::output:
        force_bit(io_port, pin_nr, value);
        set_bit(io_ddr, pin_nr);
    }
  }

  void gpio_config(gpio_port port, gpio_mode mode, uint8_t value) {
    auto& io_port = get_port(port);
    auto& io_ddr = get_ddr(port);

    switch (mode) {
      case gpio_mode::tristate:
        io_ddr = 0x00;
        io_port = 0x00;
        break;
      case gpio_mode::pullup:
        io_port = 0xFF;
        io_ddr = 0x00;
        break;
      case gpio_mode::output:
        io_ddr = value;
        io_ddr = 0xFF;
    }
  }

  void gpio_write(gpio_pin pin, bool value) {
    auto& io_port = get_port(pin.port());
    auto pin_nr = pin.pin();
    force_bit(io_port, pin_nr, value);
  }

  void gpio_write(gpio_port port, uint8_t value) { get_port(port) = value; }

  bool gpio_read(gpio_pin pin) { return get_pin(pin.port()) & _BV(pin.pin()); }

  uint8_t gpio_read(gpio_port port) { return get_pin(port); }

  bool gpio_is_output(gpio_pin pin) { return get_ddr(pin.port()) & _BV(pin.pin()); }

  bool gpio_is_any_output(gpio_port port) { return get_ddr(port); }
}  // namespace xtd
