#ifndef XTD_UC_EEPROM_HPP
#define XTD_UC_EEPROM_HPP
#include "common.hpp"

#include <avr/eeprom.h>
#include <util/atomic.h>

namespace xtd {
  template <typename T>
  class eeprom {
  public:
    eeprom(void* eeprom_address) : m_address(eeprom_address) {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { eeprom_read_block(&m_value, m_address, sizeof(T)); }
    }

    const T* operator->() const { return &m_value; }
    const T& operator*() const { return m_value; }

    void operator=(const T& value) {
      m_value = value;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { eeprom_update_block(&m_value, m_address, sizeof(T)); }
    }

  private:
    T m_value;
    void* m_address;
  };
}

#endif
