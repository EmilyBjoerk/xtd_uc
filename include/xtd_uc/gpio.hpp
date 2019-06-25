#ifndef XTD_UC_GPIO_HPP
#define XTD_UC_GPIO_HPP
#include "common.hpp"

#include <stdint.h>

namespace xtd {

  enum gpio_mode { output, tristate, pullup };

  enum gpio_port { port_b, port_c, port_d };

  class gpio_pin {
  public:
    constexpr gpio_pin(gpio_port port, uint8_t pin) : m_port_and_pin(pin | (uint8_t(port) << 6)) {}

    constexpr uint8_t pin() const { return m_port_and_pin & (0xFF >> 2); }
    constexpr gpio_port port() const { return static_cast<gpio_port>(m_port_and_pin >> 6); }

  private:
    const uint8_t m_port_and_pin;
  };

  inline constexpr auto& get_port(gpio_port port) {
    switch (port) {
      case gpio_port::port_b:
        return PORTB;
      case gpio_port::port_c:
        return PORTC;
      default:
        return PORTD;
    }
  }

  inline constexpr auto& get_ddr(gpio_port port) {
    if (port == gpio_port::port_b) {
      return DDRB;
    } else if (port == gpio_port::port_c) {
      return DDRC;
    }
    return DDRD;
  }

  inline constexpr auto& get_pin(gpio_port port) {
    if (port == gpio_port::port_b) {
      return PINB;
    } else if (port == gpio_port::port_c) {
      return PINC;
    }
    return PIND;
  }

  // Sets either the specific pin or whole port to the given GPIO mode in a safe way.
  // This is slightly slower and larger code than the '_unsafe' variants.
  inline void gpio_config(gpio_pin pin, gpio_mode mode, bool value = false) {
    const auto pin_nr = pin.pin();
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

  inline void gpio_config(gpio_port port, gpio_mode mode, uint8_t value = 0xFF) {
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

  // Sets the given value on the output pin. Must only be called when the pin is in output mode.
  // If this is called when the pin is input mode, it may change the pin from tristate to pullup.
  inline void gpio_write(gpio_pin pin, bool value) {
    auto& io_port = get_port(pin.port());
    const auto pin_nr = pin.pin();
    force_bit(io_port, pin_nr, value);
  }

  // Sets the given value on the output port. Must only be called when the port is in output mode.
  // If this is called when the port is input mode, it may change the pin from tristate to pullup.
  inline void gpio_write(gpio_port port, uint8_t value) { get_port(port) = value; }

  // Reads the value of the input pin (may be done regardless of mode, will read what's on the
  // wire)
  inline bool gpio_read(gpio_pin pin) { return get_pin(pin.port()) & _BV(pin.pin()); }

  // Reads the value of the input port (may be done regardless of mode, whill read what's on the
  // wire)
  inline uint8_t gpio_read(gpio_port port) { return get_pin(port); }

  // Returns true if the pin is set as output.
  inline bool gpio_is_output(gpio_pin pin) { return get_ddr(pin.port()) & _BV(pin.pin()); }

  // Returns true if any pin on the port is set as output.
  inline bool gpio_is_any_output(gpio_port port) { return get_ddr(port); }
}  // namespace xtd

#endif
