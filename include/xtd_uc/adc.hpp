#ifndef XTD_UC_ADC_HPP
#define XTD_UC_ADC_HPP
#include "common.hpp"
#include "cstdint.hpp"
#include "units.hpp"

namespace xtd {
  enum adc_vref : uint8_t {
#ifdef __AVR_ATtiny85__
    adc_internal_vcc = 0b000,
    adc_external_aref = 0b001,
    adc_internal_1v1 = 0b010,
    adc_internal_2v56_no_bypass = 0b110,
    adc_internal_2v56_bypass = 0b111,
#elif defined __AVR_MEGA__
    adc_external_aref = 0b00,
    adc_internal_vcc = 0b01,
    adc_internal_1_1v = 0b11,
#endif
  };

  enum adc_continuous_mode : uint8_t {
    adc_free_running = 0,
    adc_analog_comparator = 0b001,
    adc_external_irq_0 = 0b010,
    adc_tim0_cmp_match_a = 0b011,
    adc_tim0_overflow = 0b100,
    adc_tim1_cmp_match_a = 0b101,
    adc_tim1_overflow = 0b110,
    adc_tim1_capture_event = 0b111,
  };

  using adc_callback_t = void (*)(void);

  using adc_frequency = units::frequency<uint32_t, ratio<1>>;

  void adc_enable(adc_frequency adc_hz, bool msb_align_result, adc_vref vref, uint8_t ch);
  void adc_disable();
  void adc_change_speed(adc_frequency hz);
  void adc_change_vref(adc_vref vref, uint8_t ch);

  void adc_continuous_start(adc_continuous_mode mode, adc_callback_t cb);
  void adc_continuous_stop();
  void adc_dio_pin(uint8_t channel, bool enabled);

  void adc_select_ch(uint8_t channel);
  uint16_t adc_read_single_low_noise();

  void adc_await_conversion_complete();
}  // namespace xtd
#endif
