#include "xtd_uc/i2c.hpp"

#include <avr/io.h>

namespace xtd {

  enum i2c_state : char {
    idle = 0,
    addr_rx,  //
    rx_ackd,
    rx_done,
    tx_ackd,
    tx_done
  };

  i2c_device::i2c_device() {}
  i2c_device::~i2c_device() {}

  void i2c_device::on_usi_start() {
    // Start condition detected on the wire.
    // SCL is forced low untill we clear the status bit.

    ready_to_read();

    // Put us in the right USI mode (start condition IRQ disabled).
    USICR = _BV(USIOIE) |       // IRQ on Counter overflow
            (0b11 << USIWM0) |  // TWI mode with clock stretching on OVF
            (0b100 << USICLK);  // Clock USIDR on positive edge on external SCL

    expect_bits(8, addr_rx);
    release_scl();
  }

  void i2c_device::on_usi_ovf() {  // We have received or sent one frame
    switch (m_next_state) {
      case idle:
        await_start();
        break;
      case addr_rx: {
        auto addr = USIDR;
        if (m_addr == (addr & 0xFE) ||
            (addr == i2c_general_call_addr && m_respond_to_gc == i2c_general_call::enabled)) {
          bool slave_tx = addr & 1;

          // It's for us, ack it
          write(0);
          expect_bits(1, slave_tx ? tx_ackd : rx_ackd);
        } else {
          await_start();
        }
        break;
      }
      case rx_ackd:
        ready_to_read();
        expect_bits(8, rx_done);
        break;
      case rx_done:
        // Wait for user to ack or nack
        return;  // Do not release SCL
      case tx_ackd:
        break;
      default:
        break;
    }
    release_scl();
  }

  uint32_t i2c_device::enable(uint32_t) {
    PRR &= ~_BV(PRUSI);  // Make sure the USI device is powered
    PORTB |= 0b101;      // Don't manually drive the SCL and SDA pins
    DDRB &= ~0b101;      // Both pins are inputs by default
    return 0;
  }

  void i2c_device::disable() {
    PRR |= _BV(PRUSI);  // Make sure the USI device is powered down
  }

  void i2c_device::slave_on(i2c_address addr, i2c_general_call gca) {
    m_addr = addr;
    m_respond_to_gc = gca;
    await_start();
  }

  void i2c_device::slave_off() { m_addr = -1; }

  i2c_slave_state i2c_device::slave_state() const { return i2c_slave_disabled; }

  void i2c_device::slave_transmit(i2c_data /*data*/, bool /*last_byte*/) {}

  i2c_data i2c_device::slave_receive_raw() { return USIBR; }

  void i2c_device::slave_ack(i2c_read_response response) {
    write(response == i2c_read_response::ack ? 0 : -1);
    expect_bits(1, response == i2c_read_response::ack ? rx_ackd : idle);
    release_scl();
  }

  void i2c_device::release_scl() { USISR &= ~_BV(USISIF); }

  void i2c_device::expect_bits(uint8_t bits, uint8_t next_state) {
    constexpr auto mask = 0b1111;
    // The status flags are cleared by writing ones, so this is correct
    USISR = ((16 - (bits << 1)) & mask);
    m_next_state = next_state;
  }

  void i2c_device::await_start() {
    ready_to_read();
    USICR &= ~_BV(USIOIE);  // Don't trigget irq on ovf when idle
    m_next_state = idle;
  }

  void i2c_device::ready_to_read() {
    // The port pins and data direction register affect the operation of the
    // SDA and SDA pins.
    PORTB |= 0b101;   // Don't manually drive the SCL and SDA pins
    DDRB &= ~_BV(0);  // SDA is input (avoids driving SDA as data is clocked into USIDR)
    DDRB |= _BV(2);   // SCL is output (to allow clock stretching)
    USIDR = 0;        // Clear for good measure

    USICR = _BV(USISIE) |       // IRQ on Start Condition
            _BV(USIOIE) |       // IRQ on Counter overflow
            (0b11 << USIWM0) |  // TWI mode with clock stretching on ovf (15->0).
            (0b100 << USICLK);  // External clock, USIDR clocked on positive edge.
  }

  void i2c_device::write(uint8_t data) {
    // The port pins and data direction register affect the operation of the
    // SDA and SDA pins.
    PORTB |= 0b101;             // Don't manually drive the SCL and SDA pins
    DDRB |= _BV(0);             // SDA is output
    DDRB |= _BV(2);             // SCL is output (to allow clock stretching)
    USICR = _BV(USISIE) |       // IRQ on Start Condition
            _BV(USIOIE) |       // IRQ on Counter overflow
            (0b11 << USIWM0) |  // TWI mode with clock stretching on ovf (15->0).
            (0b110 << USICLK);  // External clock, USIDR clocked on negative edge.
    USIDR = data;
  }
}  // namespace xtd
