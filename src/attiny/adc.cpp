#include "xtd_uc/adc.hpp"
#include <avr/sleep.h>
#include <util/atomic.h>
#include "xtd_uc/chrono_noclock.hpp"
#include "xtd_uc/delay.hpp"

using namespace xtd::chrono_literals;

#include "../adc_common.tpp"

namespace xtd {
  constexpr uint8_t adc_vref_mask = _BV(REFS0) | _BV(REFS1) | _BV(REFS2);
  constexpr uint8_t adc_mux_mask = 0x0F;

  constexpr uint8_t adc_vref_bits(adc_vref vref) {
    return ((vref & 0x3) << REFS0) | ((vref & 0x4) << (REFS2 - 2));
  }

  static_assert(adc_vref_bits(adc_internal_2v56_bypass) == adc_vref_mask, "VRef mask are wrong");
  static_assert(adc_vref_bits(adc_internal_2v56_bypass) == 0b11010000, "VRef bits are wrong");
  static_assert(adc_vref_bits(adc_internal_2v56_no_bypass) == 0b10010000, "VRef bits are wrong");
  static_assert(adc_vref_bits(adc_internal_1v1) == 0b10000000, "VRef bits are wrong");
  static_assert(adc_vref_bits(adc_external_aref) == 0b01000000, "VRef bits are wrong");
  static_assert(adc_vref_bits(adc_internal_vcc) == 0b00000000, "VRef bits are wrong");

  void adc_enable(adc_frequency hz, bool msb_align_result, adc_vref vref, uint8_t ch) {
    PRR &= ~_BV(PRADC);                           // Power it
    ADMUX = (ch & adc_mux_mask) |                 // Select channel
            adc_vref_bits(vref) |                 // Select Vref
            (msb_align_result ? _BV(ADLAR) : 0);  // Select result alignment
    ADCSRB = 0x00;
    ADCSRA = _BV(ADEN) | adc_prescaler_bits(F_CPU, hz.count()) | _BV(ADIF);

    adc_wait_adc_cycles(1);
    adc_read_single_low_noise();
  }

  void adc_disable() {
    clr_bit(ADCSRA, ADEN);  // Disable it
    // TODO: Test if ACD is in use
    set_bit(PRR, PRADC);  // Power it down (Also powers down parts of ACD)
  }

  void adc_change_speed(uint32_t tgt_hz) {
    constexpr uint8_t dont_write_bits = _BV(ADSC) | _BV(ADIF);
    ADCSRA = (ADCSRA & ~(adc_prescaler_mask_bits | dont_write_bits)) |
             adc_prescaler_bits(F_CPU, tgt_hz);  // Set bits
  }

  void adc_change_vref(adc_vref vref, uint8_t ch) {
    uint8_t bits = adc_vref_bits(vref);
    uint8_t admux = ADMUX;
    if ((admux & adc_vref_mask) == bits) {
      return;  // Nop
    }

    // ATtiny25/45/85 datasheet 17.6: Wait 1 ADC cycle after ADSC has been set
    if (admux & _BV(ADSC)) {
      adc_wait_adc_cycles(1);
    }

    ADMUX = (admux & ~adc_vref_mask) | bits;
    // ATtiny25/45/85 datasheet table 17-4 note 2
    // delay(1_ms);

    // ATtiny25/45/85 datasheet 17.6.2: discard first read after changing vref
    // TODO: Use something faster
    adc_select_ch(ch);
    adc_read_single_low_noise();
  }

  void adc_await_conversion_complete() {
    while (ADCSRA & _BV(ADSC))
      ;  // TODO: Do something smarter than a busy wait
  }

  void adc_select_ch(uint8_t channel) {
    uint8_t admux = ADMUX;

    // ATtiny25/45/85 datasheet 17.6: Wait 1 ADC cycle after ADSC has been set
    if (admux & _BV(ADSC)) {
      adc_wait_adc_cycles(1);
    }

    ADMUX = (admux & ~adc_mux_mask) | (channel & adc_mux_mask);
  }

  uint16_t adc_read_single_low_noise() {
    // Let previous conversion finnish
    // adc_await_conversion_complete();
    auto sreg_r = SREG;
    cli();
    ADCSRA |= _BV(ADIE);
    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();
    do {
      sei();
      sleep_cpu();
      cli();
    } while (ADCSRA & _BV(ADSC));
    sleep_disable();
    uint8_t l = ADCL;
    uint8_t h = ADCH;
    ADCSRA &= ~_BV(ADIE);
    SREG = sreg_r;
    return (h << 8) | l;
  }

  void adc_dio_pin(uint8_t channel, bool enabled) {
    switch (channel) {
      case 0:
        force_bit(DIDR0, ADC0D, enabled);
        break;
      case 1:
        force_bit(DIDR0, ADC1D, enabled);
        break;
      case 2:
        force_bit(DIDR0, ADC2D, enabled);
        break;
      case 3:
        force_bit(DIDR0, ADC3D, enabled);
        break;
    }
  }

}  // namespace xtd
