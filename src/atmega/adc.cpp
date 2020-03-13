#include "xtd_uc/adc.hpp"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <stdint.h>
#include <util/atomic.h>

#include "xtd_uc/common.hpp"
#include "xtd_uc/delay.hpp"
#include "xtd_uc/utility.hpp"

static volatile xtd::adc_callback_t g_conversion_complete_cb = nullptr;

ISR(ADC_vect) {
  if (g_conversion_complete_cb) {
    g_conversion_complete_cb();
  }
}

#include "../adc_common.tpp"

using namespace xtd::unit_literals;

namespace xtd {
  constexpr uint8_t adc_vref_mask = _BV(REFS0) | _BV(REFS1);
  constexpr uint8_t adc_mux_mask = 0x0F;

  constexpr static int conv_delay_normal = 13;
  constexpr static int conv_delay_first = 25;

  static_assert(adc_frequency(100_kHz).count() == 100000, "Frequency conversion error!");

  void adc_enable(adc_frequency adc_hz, bool msb_align_result, adc_vref vref, uint8_t ch) {
    clr_bit(PRR, PRADC);  // Give ADC power in Power Reduction Register

    ADMUX = (vref << REFS0) |                      // Select voltage reference
            (msb_align_result ? _BV(ADLAR) : 0) |  // Set right align bit
            ch;                                    // Select channel
    ADCSRA = _BV(ADEN) |                           // Enable ADC
             _BV(ADIE) |                           // Enable IRQ on conversion complete.
             _BV(ADIF) |                           // Clear IRQ flag
             adc_prescaler_bits(F_CPU, adc_hz.count());
  }

  bool adc_is_enabled() { return !test_bit(PRR, PRADC) && test_bit(ADCSRA, ADEN); }

  void adc_disable() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      set_bit(ADCSRA, ADIF);  // Clear pending irq flag
      clr_bit(ADCSRA, ADEN);  // Disable ADC
      set_bit(PRR, PRADC);    // Take ADC power in Power Reduction Register
    }
  }

  void adc_change_speed(adc_frequency hz) {
    ADCSRA = (ADCSRA & 0b11101000) | adc_prescaler_bits(F_CPU, hz.count());
  }

  void adc_change_vref(adc_vref vref, uint8_t ch) {
    // Guarantee that we're one ADC Cycle after the last start conversion
    ADMUX = (ADMUX & ~adc_vref_mask) | (vref << REFS0);
    adc_select_ch(ch);
  }

  void adc_continuous_start(adc_continuous_mode mode, adc_callback_t cb) {
    g_conversion_complete_cb = cb;
    ADCSRB &= ~0b111;
    ADCSRB |= mode;
    set_bit(ADCSRA, ADATE);
    if (mode == adc_free_running) {
      set_bit(ADCSRA, ADSC);
    }
  }

  void adc_continuous_stop() {
    clr_bit(ADCSRA, ADEN);  // Disable ADC
    g_conversion_complete_cb = nullptr;
  }

  void adc_dio_pin(uint8_t channel, bool enabled) { xtd::force_bit(DIDR0, channel, enabled); }

  void adc_select_ch(uint8_t channel) {
    // Guarantee that we're one ADC Cycle after the last start conversion
    if (!test_bit(ADCSRA, ADATE) || !test_bit(ADCSRA, ADEN)) {
      // ADC is disabled or Automatic Trigger is disabled
      // Safe to write to ADMUX
      ADMUX = (ADMUX & ~adc_mux_mask) | ((channel & adc_mux_mask) << MUX0);
    } else {
      // Unsafe to select channel
    }
  }

  uint8_t adc_get_current_ch() { return (ADMUX & adc_mux_mask) >> MUX0; }

  uint16_t adc_read_single_low_noise() {
    int l, h;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      ADCSRA |= _BV(ADSC);  // Start Conversion

      // Busy wait until the conversion is done.
      // At most 25*128 = 3200 cycles
      sleep_enable();
      set_sleep_mode(SLEEP_MODE_ADC);
      while (test_bit(ADCSRA, ADSC)) {
        sei();
        sleep_cpu();
        cli();
      }
      sleep_disable();
      l = ADCL;
      h = ADCH;
    }
    return (h << 8) | l;
  }

  void adc_await_conversion_complete() {
    while (ADCSRA & _BV(ADSC))
      ;  // TODO: Do something smarter than a busy wait
  }

  inline bool isenabled() { return ADCSRA & _BV(ADEN); }

}  // namespace xtd
