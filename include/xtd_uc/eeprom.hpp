#ifndef XTD_UC_EEPROM_HPP
#define XTD_UC_EEPROM_HPP
#include "common.hpp"

#ifndef ENABLE_TEST
#include <avr/eeprom.h>
#include <util/atomic.h>
#endif

namespace xtd {
  template <typename T>
  class eeprom {
  public:
    eeprom(T* eeprom_address) : m_address(eeprom_address) {
#ifndef ENABLE_TEST
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { eeprom_read_block(&m_value, m_address, sizeof(T)); }
#else
      m_value = *eeprom_address;
#endif
    }

    const T* operator->() const { return &m_value; }
    const T& operator*() const { return m_value; }

    void operator=(const T& value) {
      m_value = value;
#ifndef ENABLE_TEST
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { eeprom_update_block(&m_value, m_address, sizeof(T)); }
#else
      *m_address = m_value;
#endif
    }

  private:
    T m_value;
    T* m_address;
  };

  template <typename T, uint16_t addr>
  class eeprom_small {
  public:
    using value_type = T;

    const T operator*() const {
      T ans;
#ifndef ENABLE_TEST
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        eeprom_read_block(&ans, reinterpret_cast<void*>(addr), sizeof(T));
      }
#else
      ans = m_value;
#endif
      return ans;
    }

    void operator=(const T& value) {
#ifndef ENABLE_TEST
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        eeprom_update_block(&value, reinterpret_cast<void*>(addr), sizeof(T));
      }
#else
      m_value = value;
#endif
    }

  private:
#ifdef ENABLE_TEST
    T m_value = 0;
#endif
  };

  template <typename T>
  class eemem {
  public:
    using value_type = T;

    constexpr eemem(T v) : m_value(v) {}

    operator value_type() const { return read(); }
    value_type get() const { return read(); }

    void operator=(const T& value) { write(value); }

  private:
    T m_value;
#ifdef ENABLE_TEST
    T read() const { return m_value; }
    void write(const T& v) { m_value = v; }
#endif
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
  };
}  // namespace xtd

#endif
