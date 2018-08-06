#ifndef XTD_UC_ADC_HPP
#define XTD_UC_ADC_HPP
#include "common.hpp"

#include <avr/io.h>
#include <stdint.h>
#include <util/atomic.h>
#include "chrono_noclock.hpp"
#include "ratio.hpp"
#include "utility.hpp"

#ifndef ADC_PRESCALER
#define ADC_PRESCALER 7
#endif

namespace xtd {
  enum adc_vref : uint8_t {
#ifdef __AVR_ATtiny85__
    adc_internal_vcc = 0b000,
    adc_external_aref = 0b001,
    adc_internal_1v1 = 0b010,
    adc_internal_2v56_no_bypass = 0b110,
    adc_internal_2v56_bypass = 0b111
#endif
  };
  void adc_enable(uint32_t adc_hz, bool msb_align_result, adc_vref vref, uint8_t ch);
  void adc_disable();
  void adc_change_speed(uint32_t hz);
  void adc_change_vref(adc_vref vref, uint8_t ch);
  void adc_dio_pin(uint8_t channel, bool enabled);

  void adc_select_ch(uint8_t channel);
  uint16_t adc_read_single_low_noise();

  void adc_await_conversion_complete();

#ifdef __AVR_MEGA__
  namespace adc {
    enum class vref : uint8_t { external_aref = 0b00, internal_vcc = 0b01, internal_1_1v = 0b11 };

    constexpr static int prescaler_select = ADC_PRESCALER;
    constexpr static int conv_delay_normal = 13;
    constexpr static int conv_delay_first = 25;

    using period = xtd::ratio<(1 << prescaler_select), F_CPU>;
    using duration = xtd::chrono::duration<int8_t, period>;

    void select_ch(uint8_t ch, vref vref, bool leftadjust = false);

    inline void enable() {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        clr_bit(PRR, PRADC);  // Give ADC power in Power Reduction Register
        ADCSRA = 0;
        set_bit(ADCSRA, ADEN);                // Enable ADC
        set_bit(ADCSRA, ADIE);                // Enable IRQ on conversion complete.
        ADCSRA |= prescaler_select << ADPS0;  // Prescale by 128 giving 125KHz ADC clock
      }
    }

    inline bool isenabled() { return ADCSRA & _BV(ADEN); }

    inline void disable() {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        clr_bit(ADCSRA, ADEN);  // Disable ADC
        set_bit(PRR, PRADC);    // Take ADC power in Power Reduction Register
      }
    }

    inline void disable_digital() {
      // Disable digital inputs from analog pins to save power.
      DIDR0 = static_cast<uint8_t>(0b111111 << ADC0D);
    }

    uint16_t blocking_read();
  }  // namespace adc
#endif
}  // namespace xtd

#endif
