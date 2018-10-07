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

  enum i2c_internal_state : char {
    state_idle,
    state_addr_rx,
    state_rx_ackd,
    state_rx_done,
    state_tx_ackd,
    state_tx_wait,
    state_tx_done
  };

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

    read_bits(8, state_addr_rx);
    USISR |= _BV(USISIF);
  }

  i2c_state i2c_device::on_usi_ovf() {
    switch (m_next_state) {
      case state_idle:
        await_start();
        return i2c_idle;
      case state_addr_rx: {
        auto addr = USIDR;
        if (is_for_us(addr)) {
          bool slave_tx = addr & 1;
          write_bits(0x00, 1, slave_tx ? state_tx_wait : state_rx_ackd);
          return i2c_busy;
        } else {
          await_start();
          return i2c_idle;
        }
      }
      case state_rx_ackd:
        read_bits(8, state_rx_done);
        return i2c_busy;
      case state_tx_done:
        read_bits(1, state_tx_ackd);  // Read the ack/nack
        return i2c_busy;
      case state_tx_ackd: {
        if ((USIDR & 0x01)) {
          await_start();
          return i2c_idle;
        }
        m_next_state = state_tx_wait;
      }  // FALLTHROUGH
      case state_tx_wait:
        if (m_tx_done) {
          transmit(0xFF, true);
          return i2c_busy;
        } else {
          // Disable overflow IRQ until user transmits something while keeping the clock stretched
          // This allows the user to return from their ISR and do some processing with IRQs enabled
          // before responding
          USICR &= ~_BV(USIOIE);
          return i2c_slave_transmit;
        }
      case state_rx_done:
        // See comment for state_tx_wait
        USICR &= ~_BV(USIOIE);
        return i2c_slave_receive;
      default:
        return i2c_internal_error;
    }
  }

  void i2c_device::power_on() {
    PRR &= ~_BV(PRUSI);  // Make sure the USI device is powered

    scl_high();
    sda_high();
    scl_output();
    sda_input();
  }

  void i2c_device::power_off() {
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

  bool i2c_device::idle() const {
    auto ovf_en = USICR & _BV(USIOIE);
    auto in_isr = USISR & (_BV(USISIF) | _BV(USIOIF));

    return !in_isr && !ovf_en;
  }

  void i2c_device::transmit(i2c_data data, bool last_byte) {
    m_tx_done = last_byte;
    write_bits(data, 8, state_tx_done);
  }

  i2c_data i2c_device::receive_raw() { return USIDR; }

  void i2c_device::ack(i2c_read_response response) {
    write_bits(response == i2c_ack ? 0x00 : 0xFF, 1,
               response == i2c_ack ? state_rx_ackd : state_idle);
  }

  void i2c_device::expect_bits(uint8_t bits, uint8_t next_state) {
    m_next_state = next_state;
    USISR = ((16 - (bits << 1)) & 0xF) | 0x70;
    USICR |= _BV(USIOIE);  // In case it was disabled
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
