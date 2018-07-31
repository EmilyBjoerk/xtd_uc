#if 0
#include <gtest/gtest.h>
#include "xtd_uc/i2c.hpp"

#include <iostream>

using namespace xtd;

i2c_device i2c(i2c_address(0x3C), 400000, i2c_general_call::enabled);

i2c_master_transmission i2c_txn;

ISR(TWINT) {
  i2c.on_irq();

  // Example asynchronous slave
  switch (i2c.slave_state()) {
    case i2c_write:
      scheduler.schedule(respond_to_write);
      // Does something like: i2c.slave_read(i2c_nack);
      break;
    case i2c_read:
      i2c.slave_write(next_byte());
      break;
    case i2c_general_call:
      scheduler.schedule(respond_to_write);
      break;
    default:
  }

  // Example asynchronous master
  switch (i2c_txn.state()) {
    case i2c_write:
      i2c_txn.write(byte);
      break;
    case i2c_read:
      auto data = i2c_txn.read(i2c_nack);
      break;
  case i2c_arbitration_lost:
    // Note that the above slave section handles if we were
    // addressed as a slave when we lost the arbitration.
    i2c_txn.reset();
    retry_transaction();
    break;
  default:
  }
}

void main() {
  i2c.enable();
  i2c.change_speed(200000);

  // Example synchronouse slave mode
  i2c.slave_on(i2c_address(0x4E), i2c_general_call::disabled);
  while (!done) {
    if (i2c_write == i2c.slave_state()) {
      // If ack is returned, another write may or may not come from the master
      auto data = i2c.read(i2c_nack);
      // do stuff with data ...
    } else if (i2c_read == i2c.slave_state()) {
      auto data = 0xFE;
      i2c.write(data, i2c_expect_ack);
    }
  }
  i2c.slave_off();

  // Example synchronous master transmission in multi-master environment
  while (retry) {
    auto txn = i2c.master_transmission();

    if (!txn.start(i2c_address(0x32), i2c_write)) {
      check_slave_state();
    }

    const uint8_t* data;
    auto written = txn.write(data, n_bytes);
    // If writes or reads fail due to arbitration the txn is marked bad and
    // subsequent calls will fail fast.
    // If it fails do write all the bytes (not acked) the written number
    // will reflect how many bytes were written.

    if (!txn.start(i2c_address(0x12), i2c_read)) {
      check_slave_state();
      continue;
    }

    uint8_t* dest;
    auto read = txn.read(dest, n_bytes);

    // Check if the transaction survived without arbitration failures (i.e. all
    // the intended wire transitions were made without collision).
    if (txn.good()) {
      retry = false;
    }
  }

  i2c.disable();
}
#endif
