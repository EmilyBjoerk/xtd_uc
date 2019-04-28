#ifndef XTD_UC_AVR_HPP
#define XTD_UC_AVR_HPP
#include "common.hpp"

#ifndef ENABLE_TEST
#include <avr/pgmspace.h>
#include <stdio.h>
#else
#include <ostream>
#define PSTR(x) x /*no-op*/

#endif

namespace xtd {
  // A simple wrapper to allow function overloading on program memory strings
  // and normal strings.
  struct pstr {
    explicit pstr(const char* s) : str(s) {}
    const char* str;

#ifdef ENABLE_TEST
    friend std::ostream& operator<<(std::ostream& os, const pstr& s) { return os << s.str; }
#endif
  };

}  // namespace xtd

#endif
