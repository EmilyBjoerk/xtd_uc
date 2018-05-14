#ifndef XTD_UC_FAKE_AVR_HPP
#define XTD_UC_FAKE_AVR_HPP
// This file defines all the control registers and their bits of the AVR family
// MCUs as global volatile variables and constants. This allows MCU code that
// manipulates these registers to be compiled for and executed on another
// architecture and also allows for such code to be testable by verifying
// how the registers are written.
//
// NOTE: All registers are initialized to 0 as opposed to what the hardware
// actually does. In your test setup you must make sure that the regisers have
// the desired value prior to exercising the CUT.

#ifdef FAKE_AVR_IMPL
#include "xtd_uc/common.hpp"
#include "xtd_uc/cstdint.hpp"
#define EXTERN /* nothing */
#define INITIALIZE = 0
#else
#include "common.hpp"
#include "cstdint.hpp"
#define EXTERN extern
#define INITIALIZE /* nothing */
#endif

EXTERN volatile uint8_t UDCON INITIALIZE;
EXTERN volatile uint8_t USB_CON INITIALIZE;
EXTERN volatile uint8_t UDIEN INITIALIZE;

constexpr uint8_t DETACH = 0;
constexpr uint8_t USBE = 7;

constexpr uint8_t SUSPE = 1;
constexpr uint8_t SOFE = 2;
constexpr uint8_t EORSTE = 3;
constexpr uint8_t WAKEUPE = 4;
constexpr uint8_t EORSME = 5;
constexpr uint8_t UPRSME = 6;

constexpr uint8_t SUSPI = 1;
constexpr uint8_t SOFI = 2;
constexpr uint8_t EORSTI = 3;
constexpr uint8_t WAKEUPI = 4;
constexpr uint8_t EORSMI = 5;
constexpr uint8_t UPRSMI = 6;

constexpr uint8_t FRZCLK = 5;

uint8_t _BV(int x) { return (1 << x); }

#endif
