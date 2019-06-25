#include "xtd_uc/common.hpp"

#include "xtd_uc/uart.hpp"

#include "xtd_uc/delay.hpp"
#include "xtd_uc/gpio.hpp"
#include "xtd_uc/queue.hpp"
#include "xtd_uc/utility.hpp"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>
#include <util/atomic.h>

#include <avr/cpufunc.h>

#if defined(UART_TX_LED_PIN) && defined(UART_TX_LED_PORT)
#define TX_LED_ENABLED
#ifndef UART_TX_LED_ACTIVE
#define UART_TX_LED_ACTIVE 0
#endif
#endif

using namespace xtd::unit_literals;

namespace xtd {
  static queue<uint8_t, uart_buffer_len> tx_queue;
  static uart_rx_callback rx_callback;

#ifdef TX_LED_ENABLED
  constexpr gpio_pin c_pin_tx_led(UART_TX_LED_PORT, UART_TX_LED_PIN);
#endif
}  // namespace xtd

ISR(USART_UDRE_vect) {
  if (!xtd::tx_queue.empty()) {
    UDR0 = xtd::tx_queue.peek();
#ifdef TX_LED_ENABLED
    xtd::gpio_write(xtd::c_pin_tx_led, UART_TX_LED_ACTIVE);
#endif
    xtd::tx_queue.pop();
  } else {
    xtd::clr_bit(UCSR0B, UDRIE0);  // No more data, disable interrupts on empty data register.
#ifdef TX_LED_ENABLED
    xtd::gpio_write(xtd::c_pin_tx_led, !UART_TX_LED_ACTIVE);
#endif
  }
}

ISR(USART_RX_vect) {
  xtd::uart_rx_status status = (UCSR0A >> UPE0) & 0x7;
  uint8_t data = UDR0;  // Data must always be read, otherwise IRQ will not be cleared.
  xtd::rx_callback(data, status);
}

namespace xtd {

  void uart_configure(uart_rx_callback rx_cb) {
    rx_callback = rx_cb;

#ifdef TX_LED_ENABLED
    xtd::gpio_config(xtd::c_pin_tx_led, xtd::gpio_mode::output, !UART_TX_LED_ACTIVE);
#endif

    // Writing of the control registers has to happen with interrupts disabled as we are enabling
    // interrupt processing. Further more when we're done we want global interrupts to remain on
    // as this is an interrupt based UART driver.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      // See table 20-1 in Atmega 328P datasheet
      using mcuratio = ratio<F_CPU, uart_baud_rate>;
      using x2ratio = ratio<(UART_SYNC ? 2 : (UART_X2 ? 8 : 16))>;
      constexpr uint16_t ubrr =
          ratio_subtract<ratio_divide<mcuratio, x2ratio>, ratio<1>>::value_round;

      // Enable power to the UART
      auto rx_en_mask = rx_cb != nullptr ? _BV(RXEN0) | _BV(RXCIE0) : 0;
      clr_bit(PRR, PRUSART0);
      UBRR0 = ubrr;
      UCSR0A = UART_X2 ? _BV(U2X0) : uint8_t();
      UCSR0B = rx_en_mask | _BV(TXEN0);

      constexpr auto paritybit_mask =
          uart_parity_bits == 0 ? 0b00 : (uart_parity_bits == 1 ? 0b10 : 0b11);
      constexpr auto stopbit_mask = uart_stop_bits == 1 ? 0b0 : 0b1;
      constexpr auto databit_mask = uart_data_bits - 5;

      UCSR0C = (paritybit_mask << UPM00) | (stopbit_mask << USBS0) | (databit_mask << UCSZ00) |
               ((UART_SYNC ? 1 : 0) << UMSEL00);
    }
  }

  void uart_disable() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      UCSR0A = 0;
      UCSR0B = 0;
      UCSR0C = 0;
      PRR |= _BV(PRUSART0);  // Kill power to UART
    }
  }

  bool uart_can_rx() { return !(PRR & _BV(PRUSART0)) && (UCSR0B & _BV(TXEN0)); }
  bool uart_can_tx() { return !(PRR & _BV(PRUSART0)) && UCSR0B & _BV(RXEN0); }

  void uart_flush() {
    NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE) {
      while (xtd::test_bit(UCSR0A, TXC0))
        ;
    }
  }

  void uart_put(char data) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      while (tx_queue.full()) {
        // The buffer is full, sleep the expected time required to empty a quarter of it.
        // We don't want to busy poll as that constant disabling of interrupts would
        // mess with the transmitter.

        sei();
        delay(uart_symbol_duration(uart_buffer_len / 4));
        cli();
      }
      tx_queue.push(data);

      // Enable interrupt processing if it was disabled.
      xtd::set_bit(UCSR0B, UDRIE0);
    }
  }
}  // namespace xtd
