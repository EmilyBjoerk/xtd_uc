#include "xtd_uc/i2c.hpp"

#include <avr/io.h>

namespace xtd {

  enum i2c_state : char {
    idle = i2c_slave_idle,
    addr_rx = 10,
    rx_ackd = 11,
    rx_done = i2c_slave_receive,
    tx_ackd = 12,
    tx_wait = i2c_slave_transmit,
    tx_done = 13
  };

  i2c_device::i2c_device() {}
  i2c_device::~i2c_device() {}

  void i2c_device::on_usi_start() {
    // Start condition detected on the wire.
    // SCL is forced low untill we clear the status bit.
    ready_to_read();
    m_tx_done = false;
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
        if (m_addr == (addr & 0xFE) || (addr == i2c_general_call_addr && m_respond_to_gc)) {
          bool slave_tx = addr & 1;

          // It's for us, ack it
          write(0);
          expect_bits(1, slave_tx ? tx_wait : rx_ackd);
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
        // Wait for user to call slave_ack()/slave_receive() (and do something with the data)
        return;  // Do not release SCL
      case tx_done:
        ready_to_read();
        expect_bits(1, tx_ackd);
        break;
      case tx_ackd: {
        bool more_data = USIDR & 1;
        if (!more_data) {
          await_start();
          break;
        }
        m_next_state = tx_wait;
      }
        // FALLTHROUGH
      case tx_wait:
        if (m_tx_done) {
          slave_transmit(0xFF, true);
        } else {
          // Wait for user to call slave_transmit
          return;  // Do not release SCL
        }
      default:
        break;
    }
    release_scl();
  }

  uint32_t i2c_device::enable(uint32_t) {
    PRR &= ~_BV(PRUSI);  // Make sure the USI device is powered

    // ATTiny datasheet 10.2.3
    DDRB &= ~0b101;  // Both pins are inputs by default
    PORTB |= 0b101;  // Don't manually drive the SCL and SDA pins
    return 0;
  }

  void i2c_device::disable() {
    slave_off();
    PRR |= _BV(PRUSI);  // Make sure the USI device is powered down
  }

  void i2c_device::slave_on(i2c_address addr, bool respond_to_gca) {
    m_addr = addr;
    m_respond_to_gc = respond_to_gca;
    await_start();
  }

  void i2c_device::slave_off() { m_addr = i2c_no_addr; }

  i2c_slave_state i2c_device::slave_state() const {
    if (m_addr == i2c_no_addr) {
      return i2c_slave_disabled;
    } else if (m_next_state == i2c_slave_idle || m_next_state == i2c_slave_transmit ||
               m_next_state == i2c_slave_receive) {
      return static_cast<i2c_slave_state>(m_next_state);
    }
    return i2c_slave_busy;
  }

  void i2c_device::slave_transmit(i2c_data data, bool last_byte) {
    m_tx_done = last_byte;
    write(data);
    expect_bits(8, tx_done);
    release_scl();
  }

  i2c_data i2c_device::slave_receive_raw() { return USIBR; }

  void i2c_device::slave_ack(i2c_read_response response) {
    write(response == i2c_ack ? 0x00 : 0xFF);
    expect_bits(1, response == i2c_ack ? rx_ackd : idle);
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
    DDRB |= _BV(0);             // SDA is output
    DDRB |= _BV(2);             // SCL is output (to allow clock stretching)
    USICR = _BV(USISIE) |       // IRQ on Start Condition
            _BV(USIOIE) |       // IRQ on Counter overflow
            (0b11 << USIWM0) |  // TWI mode with clock stretching on ovf (15->0).
            (0b110 << USICLK);  // External clock, USIDR clocked on negative edge.
    USIDR = data;
  }
}  // namespace xtd
