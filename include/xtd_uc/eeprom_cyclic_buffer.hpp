#ifndef XTD_UC_EEPROM_CYCLIC_BUFFER_HPP
#define XTD_UC_EEPROM_CYCLIC_BUFFER_HPP
#include "common.hpp"

namespace xtd {
  template <typename Element, unsigned int StartAddr, unsigned int EndAddr>
  class eeprom_cyclic_buffer {
  public:
    using size_type = unsigned int;

    size_type size() const { return (EndAddr - StartAddr) / sizeof(Element); }

    void put(){
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { eeprom_update_block(&m_value, m_address, sizeof(T)); }

    }


  private:
    
  };

}  // namespace xtd

#endif
