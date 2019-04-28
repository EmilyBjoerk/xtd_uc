#ifndef XTD_UC_UART_HPP
#define XTD_UC_UART_HPP
#include "common.hpp"

#include <avr/pgmspace.h>
#include <stdint.h>
#include "chrono_noclock.hpp"
#include "ostream.hpp"

#if UART_BAUD < 1
#define UART_BAUD 9600
#endif

#if UART_DATA_BITS < 5
#define UART_DATA_BITS 8
#endif

#ifndef UART_PARITY_BITS
#define UART_PARITY_BITS 1
#endif

#ifndef UART_STOP_BITS
#define UART_STOP_BITS 1
#endif

#ifndef UART_SYNC
#define UART_SYNC false
#endif

#ifndef UART_X2
#define UART_X2 false
#endif

namespace xtd {

  static_assert(!(UART_SYNC && UART_X2), "X2 cannot be enabled with SYNC");
  static_assert(5 <= UART_DATA_BITS && UART_DATA_BITS <= 8, "Only 5-8 bit data supported!");
  static_assert(1 == UART_STOP_BITS || 2 == UART_STOP_BITS, "Only 1 or 2 stop bits possible!");
  static_assert(0 <= UART_PARITY_BITS && UART_PARITY_BITS <= 2, "Only 0, 1 or 2 parity!");

  enum uart_rx_flags : uint8_t {
    good = 0,
    parity_error = 1,  // Level problem on wire
    data_overrun = 2,  // ISR was too slow
    frame_error = 4,   // Clock problem on wire
    overflow = 8       // RX buffer overflowed (application didn't read soon enough)
  };

  constexpr static uint8_t uart_buffer_len = 31;  // Effective size for the TRX buffers
  constexpr static uint8_t uart_data_bits = UART_DATA_BITS;
  constexpr static uint8_t uart_parity_bits = UART_PARITY_BITS;
  constexpr static uint8_t uart_stop_bits = UART_STOP_BITS;
  constexpr static uint8_t uart_frame_len = 1 + uart_data_bits + uart_parity_bits + uart_stop_bits;
  constexpr static uint32_t uart_baud_rate = UART_BAUD;

  using uart_rx_status = uint8_t;
  using uart_symbol_period = xtd::ratio<uart_frame_len, uart_baud_rate>;
  using uart_symbol_duration = typename xtd::chrono::duration<int16_t, uart_symbol_period>;
  using uart_rx_callback = void (*)(char, uart_rx_status);

  // Enables UART with the following callback on data RX. If null, RX is disabled.
  void uart_configure(uart_rx_callback rx_callback);

  // Disabled UART and removes power from it.
  void uart_disable();

  // Checks if data can be received (UART powered and configured for RX).
  // You may want to wrap this call in an atomic block.
  bool uart_can_rx();

  // Checks if data can be sent (UART powered and configured for TX).
  // You may want to wrap this call in an atomic block.
  bool uart_can_tx();

  // Blocks until the TX queue is emptied.
  void uart_flush();

  // Puts one character onto the TX queue.
  void uart_put(char c);

  struct uart_stream_tag {};

  template <>
  class ostream<uart_stream_tag> {
  public:
    void put(char c) { uart_put(c); }
  };
}  // namespace xtd

#endif
