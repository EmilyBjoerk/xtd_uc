#ifndef XTD_UC_I2C_HPP
#define XTD_UC_I2C_HPP
#include "common.hpp"

#include "cstdint.hpp"

namespace xtd {

  using i2c_address = uint8_t;
  using i2c_data = uint8_t;

  constexpr i2c_address i2c_general_call_addr = 0x00;

  enum i2c_slave_state : char{
    i2c_slave_disabled,  // Slave mode is not enabled or i2c is disabled
    i2c_slave_idle,      // Slave is not currently addressed
    i2c_slave_busy,      // Slave is busy transmitting or waiting for next command
    i2c_slave_transmit,  // Slave is expected to transmit a response to a read
    i2c_slave_receive    // Slave is receiving a write
  };

  enum i2c_master_state : char {
    i2c_master_lost_arbitration,
    i2c_master_nobody_home,
    i2c_master_next_write,
    i2c_master_write_acked,
    i2c_master_read_pending,
    i2c_master_busy,
    i2c_master_idle
  };

  enum i2c_general_call : char { enabled, disabled };
  enum i2c_read_response : char { ack, nack };
  enum i2c_txn_mode : char { write, read };

  // The master transaction class models a bus transaction as a master.
  // This includes managing the start and stop bits. The transaction may
  // become invalid at any time due to losing at arbitration to another master
  // for this reason it is important to check the validity of the transaction
  // before every interaction with it and de-use it once it has been lost.
  //
  // Of particular note, the i2c_on_slave_addressed callback may be called
  // during an interaction with the transaction if the this device lost the
  // arbitration and this device was addressed in the transaction that won.
  class i2c_master_transaction {
  public:
    i2c_master_transaction() { restart_general_call(); }
    // The address is 7 bits, MSB aligned.
    i2c_master_transaction(i2c_address addr, i2c_txn_mode mode) { restart(addr, mode); }
    ~i2c_master_transaction() { stop(); }

    // Returns the current status of the i2c master transaction.
    i2c_master_state status() const;

    // True if the master transaction is still usable
    bool good() const {
      return status() != i2c_master_lost_arbitration &&
             status() != i2c_master_nobody_home;
    }

    // Starts an address frame in the transaction. This is needed in order
    // to determine who to talk to and if it is a read or write.
    // In ordrer to send a REPEAT START event, just call start again.
    // Calling start will always produce a REPEAT START, even if the address
    // and mode are the same as the previous mode (retry transaction).
    // The address is 7 bits, MSB aligned.
    void restart(i2c_address addr, i2c_txn_mode mode);

    // Same as restart() but uses the general call instead of a specific address.
    void restart_general_call() { restart(i2c_general_call_addr, i2c_txn_mode::write); }

    // Sends a STOP condition, current ongoing transmissions are completed first.
    // Safe to call even if we don't have the bus. May block until stopped.
    void stop();

    // May only be called when status() == next_write, will always succeed
    // (data is put on the wire). Returns immediately. The value of status()
    // will change to write_acked once the transaction has been responded to
    // or lost_arbitration if the write failed due to arbitration.
    void write(i2c_data data);

    // Writes a block of data to the current slave. May only be called when
    // in write mode. Will fail fast if arbitration was lost.
    fast_size_t write_block(const uint8_t* start, const uint8_t* end) {
      auto p = start;
      while (good() && p != end) {
        if (status() == i2c_master_write_acked) {
          p++;
          if (i2c_read_response::nack == write_response()) {
            break;
          }
        }
        if (status() == i2c_master_next_write) {
          write(*p);
        }
      }
      return p - start;
    }

    // May only be called when status() == write_acked, will always return
    // the slave's response to the previous write.
    i2c_read_response write_response();

    // Must only be called if: state() == read_pending.
    // Always succeeds, reply ack if more data can be handled/makes sense
    // otherwise reply nack.
    i2c_data read(i2c_read_response response) {
      auto ans = read_raw();
      read_ack(response);
      return ans;
    }

    // Will attempt to read (end - start) bytes from the slave. May read less
    // if the arbitration was lost. Returns the actual number of bytes read.
    // Note that a slave cannot refuse a read request but it may return bogus
    // data if the read doesn't make sense.
    fast_size_t read_block(uint8_t* start, uint8_t* end) {
      auto p = start;
      while (good() && p != end) {
        if (status() == i2c_master_read_pending) {
          *p = read_raw();
          p++;
          read_ack(p == end ? i2c_read_response::nack : i2c_read_response::ack);
        }
      }
      return p - end;
    }

    // Must only be called if: state() == read_pending. Calling this method
    // does not change state().
    //
    // Always succeeds, stretches the I2C clock until read_ack() is called.
    // If read_ack() is not called after read_raw then the transaction will
    // never complete.
    i2c_data read_raw();

    // Must only be called if state() == read_pending.
    // Transmits a response (ack/nack) in response to a write into the salve.
    void read_ack(i2c_read_response response);
  };

  class i2c_device {
  public:
    i2c_device();
    ~i2c_device();

#if __AVR_ATtiny85__
    // Call this form the USI_START ISR before doing anything else.
    void on_usi_start();
    // Call this form the USI_OVF ISR before doing anything else.
    void on_usi_ovf();
#elif __AVR_MEGA__
    void on_irq();
#else
#error "Unsupported device for i2c module."
#endif
    // Powers up the hardware with the provided desired baud rate.
    // Returns the actual baud rate achieved or 0 if master mode not supported
    uint32_t enable(uint32_t master_baudrate);

    // Powers down the hardware, completes any pending transfers.
    void disable();

    // The i2c_device must be enabled prior to calling this function. If the
    // device is disabled, the slave must be re-enabled once the device is enabled.
    // Pass the address to listen on (7 bits, MSB aligned) and wheter or not to
    // react on the General Call Address (GCA).
    //
    // Please also see comments for slave_state().
    void slave_on(i2c_address addr, i2c_general_call gca);

    // Disables the slave device. Any in-flight transmissions are completed before
    // turning off.
    void slave_off();

    // Returns the current state of the slave device.
    //
    // When slave mode is on, this MUST be regularly checked, either from the
    // TWINT ISR or by polling. Failure to do so may leave the master hanging
    // indefinitely if they don't have a timeout.
    //
    // If incoming_write is returned the slave must respond with a slave_receive
    // call. If incoming_read is returned, the slave must respond with a
    // slave_transmit call.
    i2c_slave_state slave_state() const;

    // Must only be called if: slave_state() == receive
    // Always succeeds, reply ack if more data can be handled/makes sense
    // otherwise reply nack.
    i2c_data slave_receive(i2c_read_response response) {
      auto ans = slave_receive_raw();
      slave_ack(response);
      return ans;
    }

    // Must only be called if slave_state() == transmit
    // Always succeeds (data is put on the wire). If the receiver acks the
    // data slave_state() will become i2c_read again and another byte can
    // be transmitted.
    // If last_byte is true, then no more read requests will be accepted
    // until a new START condition has appeared. Will automatically respond
    // 0xFF to any and all superfluous read requests.
    void slave_transmit(i2c_data data, bool last_byte);

  private:
    // Must only be called if: slave_state() == receive
    // Always succeeds, stretches the I2C clock until slave_ack() is called.
    // If slave_ack() is not called after receive_raw then the transaction will
    // never complete (or at least until the master gives up and relinquishes the bus)
    i2c_data slave_receive_raw();

    // Must only be called if slave_state() == receive
    // Transmits a response (ack/nack) in response to a write into the salve.
    void slave_ack(i2c_read_response response);

    void expect_bits(uint8_t bits, uint8_t next_state);
    void release_scl();
    void await_start();
    void ready_to_read();
    void write(uint8_t data);

    uint8_t m_addr = -1; // 7 bits MSB aligned
    i2c_general_call m_respond_to_gc;
    uint8_t m_next_state = 0;
  };

}  // namespace xtd
#endif
