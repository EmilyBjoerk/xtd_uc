#ifndef XTD_UC_AVR_IF_HPP
#define XTD_UC_AVR_IF_HPP
// This file contains "hardware interface" layers that can be used as fakes for
// testing code outside of the target. Typically you would pass this class as a
// template parameter so that it doesn't have any overhead on the target.

namespace xtd {

  class avr_atmega8u32_real {};

  class avr_atmega8u32_fake {};

#ifdef ENABLE_TEST
  using avr_atmega8u32 = avr_atmega8u32_fake;
#else
  using avr_atmega8u32 = avr_atmega8u32_real;
#endif

}  // namespace xtd

#endif
