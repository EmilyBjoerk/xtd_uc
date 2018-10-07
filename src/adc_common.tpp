
namespace xtd {

  constexpr uint8_t adc_prescaler_max_bits = 0x07;
  constexpr uint8_t adc_prescaler_mask_bits = 0x07;

  constexpr uint8_t adc_prescaler_bits(uint32_t cpu_hz, uint32_t adc_hz) {
    uint8_t p = 1;
    const auto tgt2_hz = adc_hz << 1;
    auto actual_hz = cpu_hz >> p;
    while (actual_hz >= tgt2_hz && p < adc_prescaler_max_bits) {
      p++;
      actual_hz >>= 1;
    }
    return p;
  }

  static_assert(adc_prescaler_bits(8000000UL, 10000UL) == 0b111, "Prescaler bits wrong!");
  static_assert(adc_prescaler_bits(1280000UL, 10000UL) == 0b111, "Prescaler bits wrong!");
  static_assert(adc_prescaler_bits(1279999UL, 10000UL) == 0b110, "Prescaler bits wrong!");
  static_assert(adc_prescaler_bits(10000UL, 10000UL) == 0b001, "Prescaler bits wrong!");
  static_assert(adc_prescaler_bits(16000000UL, 100000UL) == 0b111, "Prescaler bits wrong!");

  void adc_wait_adc_cycles(uint8_t cycles) {
    uint8_t prescaler = max(1, ADCSRA & adc_prescaler_mask_bits);
    uint16_t cycles_per_adc = (1 << prescaler);

    using cpu_cycles = chrono::duration<uint16_t, ratio<1, F_CPU>>;
    delay(cpu_cycles(cycles_per_adc * cycles));
  }

}  // namespace xtd