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
}

#endif
