#ifndef XTD_UC_GPIO_HPP
#define XTD_UC_GPIO_HPP
#include "common.hpp"

#include <stdint.h>

namespace xtd {

  enum gpio_mode : uint8_t { output, tristate, pullup };

  enum gpio_port : uint8_t { port_b, port_c, port_d };

  class gpio_pin {
  public:
    constexpr gpio_pin(gpio_port port, uint8_t pin) : m_port_and_pin(pin | (uint8_t(port) << 6)) {}

    constexpr uint8_t pin() const { return m_port_and_pin & (0xFF >> 2); }
    constexpr gpio_port port() const { return static_cast<gpio_port>(m_port_and_pin >> 6); }

  private:
    const uint8_t m_port_and_pin;
  };

  // Sets either the specific pin or whole port to the given GPIO mode in a safe way.
  // This is slightly slower and larger code than the '_unsafe' variants.
  void gpio_config(gpio_pin pin, gpio_mode mode, bool value = false);
  void gpio_config(gpio_port port, gpio_mode mode, uint8_t value = 0xFF);

  // Sets the given value on the output pin. Must only be called when the pin is in output mode.
  // If this is called when the pin is input mode, it may change the pin from tristate to pullup.
  void gpio_write(gpio_pin pin, bool value);

  // Sets the given value on the output port. Must only be called when the port is in output mode.
  // If this is called when the port is input mode, it may change the pin from tristate to pullup.
  void gpio_write(gpio_port port, uint8_t value);

  // Reads the value of the input pin (may be done regardless of mode, will read what's on the
  // wire)
  bool gpio_read(gpio_pin pin);

  // Reads the value of the input port (may be done regardless of mode, whill read what's on the
  // wire)
  uint8_t gpio_read(gpio_port port);

  // Returns true if the pin is set as output.
  bool gpio_is_output(gpio_pin pin);

  // Returns true if any pin on the port is set as output.
  bool gpio_is_any_output(gpio_port port);
}

#endif
