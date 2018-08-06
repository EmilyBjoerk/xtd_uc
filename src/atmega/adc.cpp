#include "xtd_uc/common.hpp"

#include "xtd_uc/adc.hpp"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <stdint.h>
#include "xtd_uc/delay.hpp"

#ifndef ADC_PRESCALER
#define ADC_PRESCALER 7
#endif

EMPTY_INTERRUPT(ADC_vect)

namespace xtd {
  namespace adc {
    void select_ch(uint8_t ch, vref ref, bool leftadjust) {
      // Guarantee that we're one ADC Cycle after the last start conversion
      delay(duration(1));

      if (0 == (ADCSRA & _BV(ADATE)) || 0 == (ADCSRA & _BV(ADEN))) {
        // ADC is disabled or Automatic Trigger is disabled
        // Safe to write to ADMUX
        ADMUX = uint8_t((uint8_t(ref) << REFS0) |        // Use AVcc as vref
                        (leftadjust ? _BV(ADLAR) : 0) |  // Left adjust result
                        ((ch & 0b111) << MUX0));         // Select ADC channel

      } else {
        // Unsafe to select channel
      }
    }

    uint16_t blocking_read() {
      int l, h;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        ADCSRA |= _BV(ADSC);  // Start Conversion

        // Busy wait until the conversion is done.
        // At most 25*128 = 3200 cycles
        sleep_enable();
        set_sleep_mode(SLEEP_MODE_IDLE);
        while (ADCSRA & _BV(ADSC)) {
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
  }
}
