#include <avr/interrupt.h>
#include "xtd_uc/i2c.hpp"

xtd::i2c_device i2c;

volatile xtd::i2c_address addr = 0x15 << 1;
volatile xtd::i2c_data data;
const bool respond_to_gca = false;

ISR(USI_START_vect) { i2c.on_usi_start(); }

ISR(USI_OVF_vect) {
  auto state = i2c.on_usi_ovf();
  if (state == xtd::i2c_slave_receive) {
    data = i2c.receive_raw();
    i2c.ack(xtd::i2c_nack);
  } else if (state == xtd::i2c_slave_transmit) {
    data = 0xFF;
    bool done = true;
    i2c.transmit(data, done);
  }
}

int main() {
  i2c.power_on();
  i2c.slave_on(addr, respond_to_gca);
  i2c.power_off();
}
