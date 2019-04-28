#ifndef XTD_UC_I2C_HPP
#define XTD_UC_I2C_HPP
#include "common.hpp"
#include "cstdint.hpp"

namespace xtd {
  using i2c_address = uint8_t;
  using i2c_data = uint8_t;

  constexpr i2c_address i2c_no_addr = 0xFF;
  constexpr i2c_address i2c_general_call_addr = 0x00;

  enum i2c_read_response : char {
#ifdef __AVR_MEGA__
    // The hardware needs to know in advance if it should ack
    // or nack the next byte. Otherwise it would have to
    // stall and await a firmware response after each byte,
    // lowering the transmission speed notably
    i2c_ack_after_next,
    i2c_nack_after_next,
#elif defined __AVR_ATtiny85__
    i2c_ack,
    i2c_nack,
#endif
  };
  enum i2c_txn_mode : char { write, read };
  enum i2c_state : char {
    // Slave is expected to transmit a response to a read command.
    // i2c_device::transmit() must be called exactly once to unblock the bus.
    i2c_slave_transmit,

    // Slave is receiving a write and must ack/nack it.
    // i2c_receive::receive() or ack() must be called exactly once, receive_raw() may be called
    // repeatedly but will return the same value each time.
    i2c_slave_receive,

    // Arbitration lost for master transaction. User can retry transmission later.
    // No bus action is allowed.
    // This device might receive data as a slave from the winner of the arbitration, if that is the
    // case, i2c_slave_.* status will be returned from a subsequent call to on_.*.
    i2c_master_lost_arbitration,

    // No body responded to address, but we own the bus.
    // One of the following must be called exactly once to unfreeze the i2c bus:
    // * i2c_device::master_txn() - Perform repeated start condition
    // * i2c_device::master_release() - Perform stop condition
    i2c_master_nobody_home,

    // Addressed slave is ready to receive one more byte.
    // i2c_device::master_txn/master_release/transmit() may be called.
    i2c_master_transmit,

    // Addressed slave has sent us a byte.
    // One of i2c_device::receive/ack() must be called exactly once. receive_raw() may be called
    // repeadedly but will return the same value each time.
    i2c_master_receive,

    // We own the bus but no transaction is in progress.
    // One of i2c_device::master_txn/master_release() must be called.
    i2c_master_idle,

    // We do not own the bus and we are not addressed as a slave.
    i2c_idle,

    // The hardware is busy and no action is needed from the user at this point.
    i2c_busy,

    // Internal error encountered
    i2c_internal_error
  };

  // Controlls the I2C hardware on the device.
  //
  // The design is interrupt driven and buffer free, you don't pay for what you don't use.
  // If you want buffers you should implement it on on top of the funtionality offered here.
  //
  // To use make sure you call the on_.* methods from the ISRs with the matching name for your
  // device. Atleast one of the on_.* methods for your device will return a i2c_state. Based on
  // the value of the i2c_state	one or more method must be called to release the bus and continue
  // operation. See the documentation for i2c_state for more information.
  class i2c_device {
  public:
    i2c_device();
    ~i2c_device();

#if __AVR_ATtiny85__
    // Call this form the USI_START ISR before doing anything else.
    void on_usi_start();
    // Call this form the USI_OVF ISR before doing anything else.
    //
    // If the return is i2c_slave_receive exactly one call must be made to either
    // slave_ack(...) or slave_receive(...); calling slave_receive_raw() will repeatedly
    // return the value read from the wire.
    //
    // If the return value is i2c_slavc_transmit, slave_transmit(...) must be called
    // exactly once.
    //
    // Failure to do so will leave the i2c FSM in a stuck state, clock stretching SCL
    // and leave the master hanging.
    i2c_state on_usi_ovf();
#elif __AVR_MEGA__
    i2c_state on_twi();
#else
#error "Unsupported device for i2c module."
#endif
    // Powers up the hardware with the provided desired baud rate.
    // Returns the actual baud rate achieved or 0 if master mode not supported
    void power_on();

    // Powers down the hardware, completes any pending transfers.
    void power_off();

    // The i2c_device must be powered on prior to calling this function. If the
    // device is powered off, the slave must be re-enabled once the device is powered.
    // Pass the address to listen on (7 bits, MSB aligned) and wheter or not to
    // react on the General Call Address (GCA).
    void slave_on(i2c_address addr, bool respond_to_gca);

    // Disables the slave device. Any in-flight transmissions are completed before
    // turning off.
    void slave_off();

    // Return true if the driver is not doing anything, for ATtiny this means we can
    // enter deep sleep.
    bool idle() const;

    // Changes the i2c master's signaling rate. The default is standard speed: 100 kbps (100000)
    uint32_t master_speed(uint32_t bitrate);

    // Starts a master transaction with the given address and data direction.
    // Data transmission is controlled through the i2c_bus_event object returned
    // from on_.*; however it is not necessarily the case that the i2c_bus_event
    // from the next invocation will be usable for the master transmission, always
    // obey the return from i2c_bus_event::event(). In particular multiple slave
    // transactions to this device may ocurr before the i2c_bus_event indicates a
    // master transaction. The i2c_bus_event may also indicate failure by no slave
    // device acking the address or at any point during an ongoing bus transaction
    // the bus arbitration may be lost and the whole transaction should be retried
    // or abandoned later.
    void master_txn(i2c_address addr, i2c_txn_mode direction);

    // Release the bus held as master (transmit STOP condition).
    void master_release();

    // Must only be called if the on_.*_irq == receive
    // Always succeeds, reply ack if more data can be handled/makes sense
    // otherwise reply nack.
    i2c_data receive(i2c_read_response response) {
      auto ans = receive_raw();
      ack(response);
      return ans;
    }

    // Must only be called if: slave_state() == receive
    // Always succeeds, stretches the I2C clock until slave_ack() is called.
    // If slave_ack() is not called after receive_raw then the transaction will
    // never complete (or at least until the master gives up and relinquishes the bus)
    i2c_data receive_raw();

    // Must only be called if slave_state() == receive
    // Transmits a response (ack/nack) in response to a write into the salve.
    void ack(i2c_read_response response);

    // Must only be called if slave_state() == transmit
    // Always succeeds (data is put on the wire). If the receiver acks the
    // data slave_state() will become i2c_read again and another byte can
    // be transmitted.
    // If last_byte is true, then no more read requests will be accepted
    // until a new START condition has appeared. Will automatically respond
    // 0xFF to any and all superfluous read requests.
    void transmit(i2c_data data, bool last_byte);

  private:
    void expect_bits(uint8_t bits, uint8_t next_state);
    void await_start();
    void read_bits(uint8_t bits, uint8_t next_state);
    void write_bits(uint8_t data, uint8_t bits, uint8_t next_state);
    bool is_for_us(i2c_address addr) const;

    i2c_address m_addr = i2c_no_addr;  // 7 bits MSB aligned, LSB is GCE flag
    volatile uint8_t m_next_state = 0;
    volatile bool m_tx_done = false;
  };

}  // namespace xtd
#endif
