#include "xtd_uc/common.hpp"

#include "xtd_uc/gpio.hpp"
#include "xtd_uc/utility.hpp"
#include <avr/io.h>

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

  void gpio_config_unsafe(gpio_pin pin, gpio_mode mode) {
    auto pin_nr = pin.pin();
    auto& io_port = get_port(pin.port());
    auto& io_ddr = get_ddr(pin.port());

    switch (mode) {
      case gpio_mode::tristate:
        clr_bit(io_ddr, pin_nr);
        clr_bit(io_port, pin_nr);
        break;
      case gpio_mode::pullup:
        clr_bit(io_ddr, pin_nr);
        set_bit(io_port, pin_nr);
        break;
      case gpio_mode::output:
        set_bit(io_ddr, pin_nr);
    }
  }

  void gpio_config_unsafe(gpio_port port, gpio_mode mode) {
    auto& io_port = get_port(port);
    auto& io_ddr = get_ddr(port);

    switch (mode) {
      case gpio_mode::tristate:
        io_ddr = 0x00;
        io_port = 0x00;
        break;
      case gpio_mode::pullup:
        io_ddr = 0x00;
        io_port = 0xFF;
        break;
      case gpio_mode::output:
        io_ddr = 0xFF;
    }
  }

  void gpio_config(gpio_pin pin, gpio_mode mode, bool value) {
    /**
     Atmega328 datasheet 14.2.3:
       * Switching between tristate and output high.
         - Must go through either pullup or output low state.
       * Switching between pullup and output low.
         - Must go through either tristate or output high state.
     */
    switch (mode) {
      case gpio_mode::tristate:
        if (gpio_is_output(pin) && gpio_read(pin)) {
          // As we are driving the line strongly high, we will go through pull-up to tristate.
          // This is generally safe because the line is already high.
          gpio_config_unsafe(pin, gpio_mode::pullup);
        }
        break;
      case gpio_mode::pullup:
        if (gpio_is_output(pin) && !gpio_read(pin)) {
          // We are going to a state where we will be (weakly) pulling up a (possibly floating)
          // wire. We don't want to drive wire (strongly) high during the change so we go
          // through tristate.
          gpio_config_unsafe(pin, gpio_mode::tristate);
        }
        break;
      case gpio_mode::output:
        if (value && !gpio_is_output(pin) && !gpio_read(pin)) {
          // We choose to go through pullup as this will be safe as we will be driving the line
          // strongly anyway.
          gpio_config_unsafe(pin, gpio_mode::pullup);
        }
        gpio_write(pin, value);
        break;
    }
    gpio_config_unsafe(pin, mode);
  }

  void gpio_config(gpio_port port, gpio_mode mode, uint8_t value) {
    /**
     Atmega328 datasheet 14.2.3:
       * Switching between tristate and output high.
         - Must go through either pullup or output low state.
       * Switching between pullup and output low.
         - Must go through either tristate or output high state.
     */
    switch (mode) {
      case gpio_mode::tristate:
        // If any pin needs to go through, we'll make all do the transition as it's safe.
        if (gpio_is_any_output(port) && gpio_read(port)) {
          // As we are driving the line strongly high, we will go through pull-up to tristate.
          // This is generally safe because the line is already high.
          gpio_config_unsafe(port, gpio_mode::pullup);
        }
        break;
      case gpio_mode::pullup:
        if (gpio_is_any_output(port) && !gpio_read(port)) {
          // We are going to a state where we will be (weakly) pulling up a (possibly floating)
          // wire. We don't want to drive wire (strongly) high during the change so we go
          // through tristate.
          gpio_config_unsafe(port, gpio_mode::tristate);
        }
        break;
      case gpio_mode::output:
        if (value && !gpio_is_any_output(port) && !gpio_read(port)) {
          // We choose to go through pullup as this will be safe as we will be driving the line
          // strongly anyway.
          gpio_config_unsafe(port, gpio_mode::pullup);
        }
        gpio_write(port, value);
        break;
    }
    gpio_config_unsafe(port, mode);
  }

  void gpio_write(gpio_pin pin, bool value) {
    auto& io_port = get_port(pin.port());
    auto pin_nr = pin.pin();
    if (value) {
      set_bit(io_port, pin_nr);
    } else {
      clr_bit(io_port, pin_nr);
    }
  }

  void gpio_write(gpio_port port, uint8_t value) { get_port(port) = value; }

  bool gpio_read(gpio_pin pin) { return get_pin(pin.port()) & _BV(pin.pin()); }

  uint8_t gpio_read(gpio_port port) { return get_pin(port); }

  bool gpio_is_output(gpio_pin pin) { return get_ddr(pin.port()) & _BV(pin.pin()); }

  bool gpio_is_any_output(gpio_port port) { return get_ddr(port); }
}
