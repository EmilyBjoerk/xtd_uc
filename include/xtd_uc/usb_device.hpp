#ifndef XTD_UC_USB_DEVICE_HPP
#define XTD_UC_USB_DEVICE_HPP

#ifdef HAS_AVR_USB

#include "common.hpp"
#include "cstdint.hpp"
#include "ratio.hpp"

namespace xtd {
  // The different ways to power the D+ and D- pads on the MCU.
  enum class usb_pad_power : uint8_t {
    internal_3v3,  // Use this when device is powered by 3.0 - 3.6v supply
    external_3v3   // Use this when device is powered by 4.0 - 5.5v supply
  };

  // The different ways to handle PLL power during USB suspend.
  enum class usb_pll_during_suspend : uint8_t {
    power_off,  // Power down PLL during suspend
    power_on    // Keep power to PLL during suspend
  };

  // The type of USB Interrupt Service Routines (void function)
  using usb_isr = void (*)(void);

  // The types of device level USB Interrupt Requests.
  enum usb_irq : uint8_t {
    suspend,
    start_of_frame,
    end_of_reset,
    wake_up,
    end_of_resume,
    upstream_resume,
    num_irq_types
  };

  namespace detail {
    // Internal IRQ dispatch vector, don't manually modify!
    // Use usb_device::enable_irq instead
    extern usb_isr usb_isr_vector[usb_irq::num_irq_types];
  }  // namespace detail

  // User must not change the status of SUSPE and WAKEUPE IRQ flags.
  // It is assumed that global IRQs are nominally unmasked.
  template <usb_pad_power pad_power, usb_pll_during_suspend pll_suspend, int num_endpoints,
            usb_descriptor_device* desc_dev,
            usb_descriptor_configuration* desc_conf  // Only one configuration supported
            const char** desc_strs>
  class usb_device {
  public:
    // Frezes USB clock (all  and interrupts can be received)
    // and then attaches to the USB bus.
    void detach() {
      UDCON |= _BV(DETACH);
      freeze_clk();
    }

    // Unfrezes USB clock (all registers accessible and interrupts can be received)
    // and then attaches to the USB bus.
    void attach() {
      thaw_clk();
      UDCON &= ~_BV(DETACH);
    }

    // After a call to enable the following are true:
    // * USB CLK PLL is configured and has a lock
    // * USB CLK is enabled (FRZCLK=0)
    // * USB pad regulator is configured.
    // * USB is powered on (all registers accessible)
    // The following are required from the user before the USB is functional:
    // * Configure the endpoints (in particular endpoint 0)
    // * Attach the USB device by calling attach().
    void enable() {
      start_pll();
      USB_CON |= _BV(USBE);  // USB powered on
      thaw_clk();
      setup_3v3_regulator();
    }

    void enable_irq(usb_irq irq, usb_isr isr) {
      thaw_clk();  // Make sure clk is running so we can access registers.
      detail::usb_isr_vector[irq] = isr;
      UDIEN |= _BV(xtd_to_avr_irq(irq));
    }

    // Call this if you want the default suspend behaviour enabled (clock is frozen and PLL power
    // acording to pll_suspend flag).
    //
    // Otherwise you can manually enable both usb_irq::suspend and usb_irq::wake_up IRQs based on
    // (or perhaps calling) on_suspend() and on_wake_up().
    void enable_default_suspend() {
      enable_irq(usb_irq::suspend, on_suspend);
      enable_irq(usb_irq::wakeup, on_wake_up);
    }

    // Called from IRQ dispatch when bus is idle. The system task can check if is_suspended()
    // returns true and sleep the MCU with IDLE or POWER_DOWN. Global IRQ are forced on before this
    // function returns.
    static void on_suspend() {
      // It is important that this code i only ran from an ISR or with global IRQ disabled and
      // enabled immediately after this returns. Otherwise we might have a race where we got a
      // suspend IRQ immediately followed by a async IRQ that was supposed to wake the device
      // but that IRQ interrupted us in the process of suspending and we then would end up wrongly
      // suspending after handling that event. By essentially having IRQ disabled during this
      // on_suspend() function we force any such wake IRQ to wait until we are suspended and then
      // immediately wake us up.

      UDIEN |= _BV(WAKEUPE);  // Make darn sure we can be woken up.

      // ...and if we're configured for it,  stop PLL.
      if (pll_suspend == usb_pll_during_suspend::power_off) {
        stop_pll();
      }
      freeze_clk();
      sei();  // Global IRQ must be enabled or we can not wake up.
    }

    static void on_wake_up() {
      if (pll_suspend == usb_pll_during_suspend::power_off) {
        start_pll();
      }
      thaw_clk();
    }

    void is_suspended() const { return USB_CON & _BV(FRZCLK); }

    // Resets the USB device. In particular:
    // * USB clock is frozen
    // * USB is detached
    // * Endpoints banks are reset
    // * Internal state is reset
    // * PLL maintains previous state.
    void reset() { USB_CON &= _BV(USBE); }

    void disable() {
      // detach(); Shouldn't be needed as hardware detaches automatically on reset.
      reset();
      stop_pll();
    }

  private:
    constexpr static auto cpu_hz = F_CPU;
    constexpr static auto usb_hz = 48000000;
    constexpr static auto pll_mult = 6;

    // Scale factor: usb_hz = F_CPU * scale * pll_mult
    // scale = usb_hz / (F_CPU*pll_mult)
    using scale_factor = xtd::ratio<usb_hz, cpu_hz * pll_mult>;

    static_assert(scale_factor::den == 1, "Scale factor must be integer!");
    static_assert(scale_factor::num == 1 || scale_factor::num == 2 || scale_factor::num == 3 ||
                      scale_factor::num == 5,
                  "Only scale factor 1, 2, 4 and 5 are supported!");

    uint8_t xtd_to_avr_irq(usb_irq irq) {
      switch (irq) {
        case usb_irq::suspend:
          return SUSPE;
        case usb_irq::start_of_frame:
          return SOFI;
        case usb_irq::end_of_reset:
          return EORSTI;
        case usb_irq::wake_up:
          return WAKEUPI;
        case usb_irq::end_of_resume:
          return EORSMI;
        case usb_irq::upstream_resume:
          return UPRSMI;
        default:
          return 0;
      }
    }

    void setup_3v3_regulator() {
      if (pad_power == usb_pad_power::external_3v3) {
        REGCR |= _BV(REGDIS);
      } else {  // internal_3v3
        REGCR &= ~_BV(REGDIS);
      }
    }

    void freeze_clk() { USB_CON |= _BV(FRZCLK); }

    void thaw_clk() {
      USB_CON &= _BV(FRZCLK);  // Enable access to all USB hardware for conjiguration
    }

    void start_pll() {
      // Setup prescaler
      if (scale_factor::num == 1) {
        PLLCSR = 0;
      } else if (scale_factor::num == 2) {
        PLLCSR = _BV(PLLP0);  // Yes, assignment, because we want to make sure PLL is off as well.
      } else if (scale_factor::num == 3) {
        PLLCSR = _BV(PLLP1);
      } else if (scale_factor::num == 5) {
        PLLCSR = _BV(PLLP2);
      }

      // Enable PLL
      PLLCSR = _BV(PLLE);

      // Wait for lock
      while (!(PLLCSR & _BV(PLOCK))) {
        // nop
      }
    }

    void stop_pll() { PLL_CSR = 0; }
  };  // namespace xtd
}  // namespace xtd

#endif

#endif
