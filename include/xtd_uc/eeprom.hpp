#ifndef XTD_UC_EEPROM_HPP
#define XTD_UC_EEPROM_HPP
#include "common.hpp"
#include "utility.hpp"

#ifndef ENABLE_TEST
#include <avr/eeprom.h>
#include <util/atomic.h>
#endif

// Make rtags happy
#ifndef EEMEM
#define EEMEM
#endif

namespace xtd {
  template <typename T>
  class eemem {
  public:
    using value_type = T;

    template <typename... U>
    constexpr eemem(U&&... v) : m_value(xtd::forward<U>(v)...) {}

    operator value_type() const { return read(); }
    value_type get() const { return read(); }

    auto operator*() { return get(); }

    template<typename U>
    void operator=(U&& value) { write(value); }

  private:
    T m_value;
#ifdef ENABLE_TEST
    T read() const { return m_value; }
    void write(const T& v) { m_value = v; }
#else
    T read() const {
      T ans;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        eeprom_read_block(&ans, const_cast<void*>(reinterpret_cast<const void*>(&m_value)),
                          sizeof(T));
      }
      return ans;
    }
    void write(const T& v) {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        eeprom_update_block(&v, reinterpret_cast<void*>(&m_value), sizeof(T));
      }
    }
#endif
  };
}  // namespace xtd

#endif
