#include "xtd_uc/i2c.hpp"

#include "xtd_uc/algorithm.hpp"
#include "xtd_uc/cmath.hpp"
#include "xtd_uc/utility.hpp"

#include <avr/io.h>

namespace xtd {
  // ---------------------------------------------------------------------------
  // Implementation of: i2c_master_transaction
  // ---------------------------------------------------------------------------

  i2c_master_state i2c_master_transaction::status() const { return i2c_master_idle; }
  void i2c_master_transaction::restart(i2c_address /*addr*/, i2c_txn_mode /*mode*/) {}
  void i2c_master_transaction::stop() {}
  void i2c_master_transaction::write(i2c_data /*data*/) {}
  i2c_read_response i2c_master_transaction::write_response() { return i2c_nack; }
  i2c_data i2c_master_transaction::read_raw() { return 0; }
  void i2c_master_transaction::read_ack(i2c_read_response /*response*/) {}

  // ---------------------------------------------------------------------------
  // Implementation of: i2c_device
  // ---------------------------------------------------------------------------

  i2c_device::i2c_device() {}
  i2c_device::~i2c_device() {}

  void i2c_device::on_irq() {}

  uint32_t i2c_device::enable(uint32_t bitrate) {
    clr_bit(PRR, PRTWI);

    // bitrate = CPU_FREQ/(16 + 2 * TWBR * Prescaler)
    // TWBR * Prescaler = (CPU_FREQ / (bitrate) - 16)/2
    // TWBR * Prescaler = (CPU_FREQ / (2*bitrate) - 8)
    // OR: bitrate * TWBR * Prescaler = CPU_FREQ/2 - 8*bitrate

    // Try all prescaler values
    if (bitrate * 16 > F_CPU) {
      bitrate = F_CPU / 16;
    }

    uint32_t best_error = -1;
    uint8_t best_twbr;
    uint8_t best_prescaler;
    for (uint8_t pb = 1; pb < 4; pb++) {
      auto p = (1 << (2 * pb));
      auto twbr = divide<uint32_t, round_style::nearest>(F_CPU - 16 * bitrate, 2 * p * bitrate);
      clamp<decltype(twbr)>(twbr, 0, 255);
      auto br = F_CPU / (16 + 2 * twbr * p);
      auto abs_error = br < bitrate ? bitrate - br : br - bitrate;
      if (abs_error < best_error) {
        best_error = abs_error;
        best_prescaler = pb;
        best_twbr = twbr;
      }
    }

    TWBR = best_twbr;
    TWSR = best_prescaler;
    TWCR = _BV(TWEN) | _BV(TWIE);  // Master mode only enabled initially.
    return F_CPU / (16 + 2 * best_twbr * best_prescaler);
  }

  void i2c_device::disable() {
    // TODO: Wait until ongoing transactions finish and transmit stop condition
    TWCR &= ~_BV(TWEN);
    PRR |= _BV(PRTWI);
  }

  void i2c_device::slave_on(i2c_address addr, bool respond_to_gca) {
    TWAR = addr + (respond_to_gca ? 1 : 0);
    set_bit(TWCR, TWEA);  // Enable acking slave address.
  }

  void i2c_device::slave_off() {
    clr_bit(TWCR, TWEA);  // Enable acking slave address.
  }

  /*  i2c_slave_state i2c_device::slave_state() const {
    if (test_bit(PRR, PRTWI) || test_bit(TWCR, TWEN)) {
      return i2c_slave_disabled;
    }
    // auto st = TWSR >> 3;  // 5 bits, 32 values
    // switch (st) { case:
    return i2c_slave_disabled;
    }*/

  i2c_data i2c_device::slave_receive_raw() { return TWDR; }

  void i2c_device::slave_ack(i2c_read_response response) {
    if (response == i2c_ack) {
      set_bit(TWCR, TWEA);
    } else {
      clr_bit(TWCR, TWEA);
    }
    set_bit(TWCR, TWINT);
  }

  void i2c_device::slave_transmit(i2c_data data, bool last_byte) {
    TWDR = data;
    if (last_byte) {
      clr_bit(TWCR, TWEA);
    } else {
      set_bit(TWCR, TWEA);
    }
    set_bit(TWCR, TWINT);
  }
}  // namespace xtd
