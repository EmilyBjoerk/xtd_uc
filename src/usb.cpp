#include "xtd_uc/usb.hpp"
#include "xtd_uc/common.hpp"

namespace xtd {

  namespace detail {

    static usb_isr usb_isr_vector[usb_irq::num_irq_types] = {};

    void usb_gen_dispatch(uint8_t irq_avr, usb_irq irq_xtd) {
      // Make sure USB clock is running during IRQ so registers are accessible.
      USB_CON &= ~_BV(FRZCLK);

      if (UDINT & _BV(irq_avr)) {
        if (usb_isr_vector[irq_xtd]) {
          usb_isr_vector[irq_xtd]();
        }
        UDINT &= ~_BV(irq_avr);
      }
    }
  }  // namespace detail

}  // namespace xtd

extern "C" {

// General IRQ handler, simply dispatch to software IRQ vector (costs us 12 bytes of SRAM)
void USB_GEN_vect(void) __attribute__((signal)) {
  using namespace xtd::detail;
  usb_gen_dispatch(SUSPI, xtd::usb_irq::suspend);
  usb_gen_dispatch(SOFI, xtd::usb_irq::start_of_frame);
  usb_gen_dispatch(EORSTI, xtd::usb_irq::end_of_reset);
  usb_gen_dispatch(WAKEUPI, xtd::usb_irq::wake_up);
  usb_gen_dispatch(EORSMI, xtd::usb_irq::end_of_resume);
  usb_gen_dispatch(UPRSMI, xtd::usb_irq::upstream_resume);
}

// Endpoint/pipe IRQ
void USB_COM_vect(void) __attribute__((signal)) {}
}

namespace xtd {}
