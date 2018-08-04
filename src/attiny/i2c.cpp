#include "xtd_uc/i2c.hpp"

#include <avr/io.h>

namespace xtd {

  void sda_input() { DDRB &= ~_BV(PB0); }
  void sda_output() { DDRB |= _BV(PB0); }
  void sda_low() { PORTB &= ~_BV(PB0); }
  void sda_high() { PORTB |= _BV(PB0); }

  void scl_input() { DDRB &= ~_BV(PB2); }
  void scl_output() { DDRB |= _BV(PB2); }
  void scl_low() { PORTB &= ~_BV(PB2); }
  void scl_high() { PORTB |= _BV(PB2); }

  enum i2c_state : char { idle, addr_rx, rx_ackd, rx_done, tx_ackd, tx_wait, tx_done };

  i2c_device::i2c_device() {}
  i2c_device::~i2c_device() {}

  void i2c_device::on_usi_start() {
    m_tx_done = false;

    // Wait for SCL to go low to ensure the "Start Condition" has completed.
    while ((PINB & _BV(PB2)) & !(USISR & (1 << USIPF)))
      ;

    USICR = _BV(USISIE) |       // IRQ on Start Condition
            _BV(USIOIE) |       // IRQ on Counter overflow
            (0b11 << USIWM0) |  // TWI mode with clock stretching on ovf (15->0).
            (0b100 << USICLK);  // External clock, USIDR clocked on positive edge.

    read_bits(8, addr_rx);
    USISR |= _BV(USISIF);
  }

  i2c_slave_state i2c_device::on_usi_ovf() {
    switch (m_next_state) {
      case idle:
        await_start();
        return i2c_slave_idle;
      case addr_rx: {
        auto addr = USIDR;
        if (is_for_us(addr)) {
          bool slave_tx = addr & 1;
          write_bits(0x00, 1, slave_tx ? tx_wait : rx_ackd);
          return i2c_slave_busy;
        } else {
          await_start();
          return i2c_slave_idle;
        }
      }
      case rx_ackd:
        read_bits(8, rx_done);
        return i2c_slave_busy;
      case tx_done:
        read_bits(1, tx_ackd);  // Read the ack/nack
        return i2c_slave_busy;
      case tx_ackd: {
        bool more_data = USIDR & 1;
        if (!more_data) {
          await_start();
          return i2c_slave_idle;
        }
        m_next_state = tx_wait;
      }  // FALLTHROUGH
      case tx_wait:
        if (m_tx_done) {
          slave_transmit(0xFF, true);
          return i2c_slave_busy;
        } else {
          return i2c_slave_transmit;
        }
      case rx_done:
        return i2c_slave_receive;
      default:
        return i2c_slave_internal_error;
    }
  }

  uint32_t i2c_device::enable(uint32_t) {
    PRR &= ~_BV(PRUSI);  // Make sure the USI device is powered

    scl_high();
    sda_high();
    scl_output();
    sda_input();
    return 0;
  }

  void i2c_device::disable() {
    slave_off();
    PRR |= _BV(PRUSI);  // Make sure the USI device is powered down
  }

  void i2c_device::slave_on(i2c_address addr, bool respond_to_gca) {
    m_addr = addr | (respond_to_gca ? 1 : 0);
    await_start();
  }

  void i2c_device::slave_off() {
    m_addr = i2c_no_addr;
    scl_high();
    sda_high();
    scl_input();
    sda_input();
    USICR = 0x00;  // Disable USI
    USISR = 0xF0;  // Clear all flags and reset overflow counter
  }

  void i2c_device::slave_transmit(i2c_data data, bool last_byte) {
    m_tx_done = last_byte;
    write_bits(data, 8, tx_done);
  }

  i2c_data i2c_device::slave_receive_raw() { return USIDR; }

  void i2c_device::slave_ack(i2c_read_response response) {
    write_bits(response == i2c_ack ? 0x00 : 0xFF, 1, response == i2c_ack ? rx_ackd : idle);
  }

  void i2c_device::expect_bits(uint8_t bits, uint8_t next_state) {
    m_next_state = next_state;
    USISR = ((16 - (bits << 1)) & 0xF) | 0x70;
  }

  void i2c_device::await_start() {
    DDRB &= ~_BV(PB0);          // SDA as input
    USICR = _BV(USISIE) |       // IRQ on Start Condition
            (0b10 << USIWM0) |  // TWI mode with clock stretching on start only
            (0b100 << USICLK);  // External clock, USIDR clocked on positive edge.
    USISR = 0x70;               // Clear all flags except start cond and reset counter
  }

  void i2c_device::read_bits(uint8_t bits, uint8_t next_state) {
    DDRB &= ~_BV(PB0);  // SDA as input
    expect_bits(bits, next_state);
  }

  void i2c_device::write_bits(uint8_t data, uint8_t bits, uint8_t next_state) {
    USIDR = data;  // SDA as output
    DDRB |= _BV(PB0);
    expect_bits(bits, next_state);
  }

  bool i2c_device::is_for_us(i2c_address addr) const {
    // This is slightly shorter machine code than: (m_addr & 0xFE) == (addr & 0xFE)
    auto addr_match = !((m_addr ^ addr) >> 1);
    auto gc_enabled = m_addr & 0x01;
    auto gc = !addr;
    return addr_match || (gc_enabled && gc);
  }
}  // namespace xtd
