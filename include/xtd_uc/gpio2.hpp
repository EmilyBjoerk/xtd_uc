#ifndef XTD_UC_GPIO2_HPP
#define XTD_UC_GPIO2_HPP
#include "common.hpp"
#include "cstdint.hpp"

namespace xtd {
  enum gpio_port { port_b, port_c, port_d };

  namespace detail {
    template <gpio_port P>
    struct get_port;
    template <>
    struct get_port<port_b> {
      static auto& get() { return PORTB; }
    };
    template <>
    struct get_port<port_c> {
      static auto& get() { return PORTC; }
    };
    template <>
    struct get_port<port_d> {
      static auto& get() { return PORTD; }
    };

    template <gpio_port P>
    struct get_ddr;
    template <>
    struct get_ddr<port_b> {
      static auto& get() { return DDRB; }
    };
    template <>
    struct get_ddr<port_c> {
      static auto& get() { return DDRC; }
    };
    template <>
    struct get_ddr<port_d> {
      static auto& get() { return DDRD; }
    };
  }  // namespace detail

  template <gpio_port P, int N>
  class pin {
  public:
    pin() = delete;

    __attribute__((always_inline)) static bool test() {
      return test_bit(detail::get_port<P>::get(), N);
    }

    __attribute__((always_inline)) static void set() { set_bit(detail::get_port<P>::get(), N); }

    __attribute__((always_inline)) static void clr() { clr_bit(detail::get_port<P>::get(), N); }

    __attribute__((always_inline)) static void toggle() {
      toggle_bit(detail::get_port<P>::get(), N);
    }

    __attribute__((always_inline)) static void set(bool v) {
      force_bit(detail::get_port<P>::get(), N, v);
    }

    __attribute__((always_inline)) static void tristate() {
      clr_bit(detail::get_ddr<P>::get(), N);
      clr_bit(detail::get_port<P>::get(), N);
    }

    __attribute__((always_inline)) static void pullup() {
      set_bit(detail::get_port<P>::get(), N);
      clr_bit(detail::get_ddr<P>::get(), N);
    }

    __attribute__((always_inline)) static void output(bool value) {
      set(value);
      set_bit(detail::get_ddr<P>::get(), N);
    }
  };

  template <gpio_port P>
  class port {
  public:
    port() = delete;

    __attribute__((always_inline)) static auto test() { return detail::get_port<P>::get(); }

    __attribute__((always_inline)) static void set(uint8_t pins) {
      detail::get_port<P>::get() |= pins;
    }

    __attribute__((always_inline)) static void clr(uint8_t pins) {
      detail::get_port<P>::get() &= ~pins;
    }

    __attribute__((always_inline)) static void toggle(uint8_t pins) {
      detail::get_port<P>::get() ^= pins;
    }

    __attribute__((always_inline)) static void tristate(uint8_t pins) {
      detail::get_ddr<P>::get() &= ~pins;
      detail::get_port<P>::get() &= ~pins;
    }

    __attribute__((always_inline)) static void pullup(uint8_t pins) {
      detail::get_port<P>::get() |= pins;
      detail::get_ddr<P>::get() &= ~pins;
    }

    __attribute__((always_inline)) static void output(uint8_t pins, uint8_t values) {
      detail::get_port<P>::get() &= ~pins;
      detail::get_port<P>::get() |= (pins & values);
      detail::get_ddr<P>::get() |= pins;
    }
  };
}  // namespace xtd

#endif
