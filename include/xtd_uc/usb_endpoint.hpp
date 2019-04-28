#ifndef XTD_UC_USB_ENDPOINT_HPP
#define XTD_UC_USB_ENDPOINT_HPP

#ifdef HAS_AVR_USB

#include "common.hpp"
#include "cstdint.hpp"
#include "ratio.hpp"

namespace xtd {

  // IN is into the host (out from the device)
  // OUT is out from the host (in to the device)
  enum usb_endpoint_type : uint8_t {
    control = 0b00000000,
    isochronous_in = 0b01000001,
    isochronous_out = 0b01000000,
    bulk_in = 0b10000001,
    bulk_out = 0b10000000,
    interrupt_in = 0b11000001,
    interrupt_out = 0b11000000
  };

  enum usb_endpoint_size : uint8_t {
    one_by_8 = 0b00000010,
    two_by_8 = 0b00000110,
    one_by_16 = 0b00010010,
    two_by_16 = 0b00010110,
    one_by_32 = 0b00100010,
    two_by_32 = 0b00100110,
    one_by_64 = 0b00110010,
    two_by_64 = 0b00110110,
  };

  class usb_endpoint {
  public:
    void reset() { UERST }
    void select() { UENUM = endpoint_num; }

    bool activate(usb_endpoint_type type, usb_endpoint_size size) {
      select();

      UECONX |= _BV(EPEN);
      UECFG0X = type;
      UECFG1X = size;

      return 0 != (UESTA0X & _BV(CFGOK));
    }

    void deactivate();
    void stall();
    void recover();

    void handle_abort() {
      // Procedure frmo 21.
      /*
    UEIENX = 0;
    TXINE = 0;
    while (!NBUSYBK) {
      KILLBK = 1;
      while (KILLLBK) {
        // nop
      }
    }
    reset();
      */
    }

    // Returns: For an IN endpoint the number of bytes written to the current bank.
    // For OUT endpoints the number of bytes left in the bank that have not been read.
    uint8_t bytes() const { return UEBCLX; }

    // Reads the next byte from the endpoint's buffer. Make sure bytes() > 0.
    uint8_t get() const { return UEDATX; }

    // Reads the next byte from the endpoint's buffer. Make sure bytes() < BANK_SIZE.
    void put(uint8_t data) { UEDATX = data; }

    void bank_done() { /*FIFOCON = 0;*/
    }

    static void on_setup(usb_endpoint& control_ep, usb_device& dev) {
      // nop

      control_ep.select();

      UADD = control_ep.get();

      ADDEN = 1;
    }

  private:
    uint8_t endpoint_num;
  };
}  // namespace xtd

#endif

#endif
