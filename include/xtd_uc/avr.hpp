#ifndef XTD_UC_AVR_HPP
#define XTD_UC_AVR_HPP
#include "common.hpp"

#ifdef ENABLE_TEST
#include <ostream>
#else
#include <avr/pgmspace.h>
#include <stdio.h>
#endif

// Make clang tooling shut up
#ifndef PSTR
#define PSTR(x) x
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
