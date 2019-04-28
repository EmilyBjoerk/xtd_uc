#include "xtd_uc/i2c.hpp"

#include "xtd_uc/algorithm.hpp"
#include "xtd_uc/cmath.hpp"
#include "xtd_uc/utility.hpp"

#include <avr/io.h>
#include <util/atomic.h>

namespace xtd {

  enum atmegaxx8twi_status : uint8_t {
    twi_start_cond_complete = 0x08,
    twi_stop_cond_received = 0xA0,
    twi_rep_start_complete = 0x10,
    twi_lost_arbitration = 0x38,

    twi_sla_r_acked = 0x40,
    twi_sla_r_nacked = 0x48,
    twi_sla_w_acked = 0x18,
    twi_sla_w_nacked = 0x20,

    twi_mt_data_ack_received = 0x28,
    twi_mt_data_nack_received = 0x30,
    twi_mr_data_ack_returned = 0x50,
    twi_mr_data_nack_returned = 0x58,

    twi_sr_addressed = 0x60,
    twi_sr_addressed_lost_arb = 0x68,
    twi_sr_data_ack_returned = 0x80,
    twi_sr_data_nack_returned = 0x88,

    twi_st_addressed = 0xA8,
    twi_st_addressed_lost_arb = 0xB0,
    twi_st_data_ack_received = 0xB8,
    twi_st_data_nack_received = 0xC0,
    twi_st_last_data_ack_received = 0xC8,

    twi_gc_addressed = 0x70,
    twi_gc_addressed_lost_arb = 0x78,
    twi_gc_data_ack_returned = 0x90,
    twi_gc_data_nack_returned = 0x98,
  };

  void stretch_scl() {
    // Clock is stretched by not clearing TWINT but in order to prevent repeated
    // triggering of the TWI IRQ we need to disable irqs for TWI without clearing
    // TWINT
    clr_bit(TWCR, TWIE);
  }

  void release_scl() {
    set_bit(TWCR, TWINT);  // Clear interrupt flag
    set_bit(TWCR, TWIE);   // In case we were stretching the clock outside of the ISR
  }

  i2c_state stretch_scl(i2c_state s) {
    stretch_scl();
    return s;
  }

  i2c_state release_scl(i2c_state s) {
    release_scl();
    return s;
  }

  void start_condition() {
    // set_bit(TWCR, TWEN); // Must be powered on

    set_bit(TWCR, TWSTA);  // Request start condition
  }

  void stop_condition() {
    // set_bit(TWCR, TWEN); // Must be powered on
    // set_bit(TWCR, TWIE); // We always have irqs enabled

    set_bit(TWCR, TWSTO);  // Request stop bit
  }

  volatile i2c_address g_slave_addr = 0;

  i2c_device::i2c_device() {}
  i2c_device::~i2c_device() {}

  i2c_state i2c_device::on_twi() {
    // Clear TWSTA when start condition generate
    auto status = TWSR & 0xF8;
    switch (status) {
      //
      // We started a new transmission as master, the slave address is stored in g_slave_addr
      //
      case twi_start_cond_complete:  // FALLTHROUGH
      case twi_rep_start_complete:
        TWDR = g_slave_addr;
        clr_bit(TWCR, TWSTA);
        return release_scl(i2c_busy);

      //
      // Bus Arbitration
      //
      case twi_lost_arbitration:
        return release_scl(i2c_master_lost_arbitration);

      //
      // Address was nacked
      //
      case twi_sla_r_nacked:  // FALLTHROUGH
      case twi_sla_w_nacked:
        // User must call one of:
        // * i2c_device::master_txn()
        // * i2c_device::master_release()
        // Which will release the stretch
        return stretch_scl(i2c_master_nobody_home);

      //
      // Master transmitter
      //
      case twi_sla_w_acked:  // FALLTHROUGH
      case twi_mt_data_ack_received:
        // User must call one of:
        // * i2c_device::master_txn()
        // * i2c_device::master_release()
        // * i2c_device::transmit()
        return stretch_scl(i2c_master_transmit);
      case twi_mt_data_nack_received:
        // User must call one of:
        // * i2c_device::master_txn()
        // * i2c_device::master_release()
        return stretch_scl(i2c_master_idle);

      //
      // Master receiver
      //
      case twi_sla_r_acked:  // FALLTHROUGH
      case twi_mr_data_ack_returned:
        // User must call one of:
        // * i2c_device::master_txn()
        // * i2c_device::master_release()
        // * i2c_device::receive()
        // * i2c_device::ack()
        // Additionally user may call the following any number of times
        // before calling the above:
        // * i2c_device::receive_raw()
        return stretch_scl(i2c_master_receive);
      case twi_mr_data_nack_returned:
        // User must call one of:
        // * i2c_device::master_txn()
        // * i2c_device::master_release()
        return stretch_scl(i2c_master_idle);

      //
      // Slave Receiver
      //
      case twi_sr_addressed:           // FALLTHROUGH
      case twi_sr_addressed_lost_arb:  // FALLTHROUGH
      case twi_gc_addressed:           // FALLTHROUGH
      case twi_gc_addressed_lost_arb:
        return release_scl(i2c_busy);

      case twi_sr_data_ack_returned:  // FALLTHROUGH
      case twi_gc_data_ack_returned:
        // User must call:
        // * i2c_device::receive()
        // * i2c_device::ack()
        // Additionally user may call the following any number of times
        // before calling the above:
        // * i2c_device::receive_raw()
        return stretch_scl(i2c_slave_receive);

      case twi_sr_data_nack_returned:  // FALLTHROUGH
      case twi_gc_data_nack_returned:
        return release_scl(i2c_busy);

      case twi_stop_cond_received:
        return release_scl(i2c_busy);

        // Slave Transmitter
      case twi_st_addressed:           // FALLTHROUGH
      case twi_st_addressed_lost_arb:  // FALLTHROUGH
      case twi_st_data_ack_received:
        // User must call:
        // * i2c_device::transmit
        return stretch_scl(i2c_slave_transmit);

      case twi_st_data_nack_received:  // FALLTHROUGH
      case twi_st_last_data_ack_received:
        return release_scl(i2c_busy);
    };

    return i2c_internal_error;
  }

  void i2c_device::power_on() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      clr_bit(PRR, PRTWI);  // Power up the IO

      TWCR = _BV(TWINT) |  // Clear any pending IRQ
             _BV(TWEN) |   // Enable TWI (SDA/SCL pins controlled by TWI HW)
             _BV(TWIE);    // Enable IRQs
    }
    master_speed(100000);  // Standard Speed

    TWCR = _BV(TWEN) | _BV(TWIE);  // Master mode only enabled initially.
  }

  void i2c_device::power_off() {
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

  bool i2c_device::idle() const { return false; }

  uint32_t master_speed(uint32_t bitrate) {
    // bitrate = CPU_FREQ/(16 + 2 * TWBR * Prescaler)
    // TWBR * Prescaler = (CPU_FREQ / (bitrate) - 16)/2
    // TWBR * Prescaler = (CPU_FREQ / (2*bitrate) - 8)
    // OR: bitrate * TWBR * Prescaler = CPU_FREQ/2 - 8*bitrate

    // Try all prescaler values
    if (bitrate * 16 > F_CPU) {
      bitrate = F_CPU / 16;
    }

    uint32_t best_error = 0xFFFFFFFF;
    uint8_t best_twbr = 1;
    uint8_t best_prescaler = 1;
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
    return F_CPU / (16 + 2 * best_twbr * best_prescaler);
  }

  void i2c_device::master_txn(i2c_address addr, i2c_txn_mode direction) {
    g_slave_addr = (addr << 1) | direction;
    start_condition();
    release_scl();
  }

  void i2c_device::master_release() {
    stop_condition();
    release_scl();
  }

  i2c_data i2c_device::receive_raw() { return TWDR; }

  void i2c_device::ack(i2c_read_response response) {
    if (response == i2c_ack_after_next) {
      set_bit(TWCR, TWEA);
    } else {
      clr_bit(TWCR, TWEA);
    }
    release_scl();
  }

  void i2c_device::transmit(i2c_data data, bool last_byte) {
    TWDR = data;
    if (last_byte) {
      clr_bit(TWCR, TWEA);
    } else {
      set_bit(TWCR, TWEA);
    }
    release_scl();
  }
}  // namespace xtd
