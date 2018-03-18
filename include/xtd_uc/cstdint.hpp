#ifndef XTD_UC_CSTDINT_HPP
#define XTD_UC_CSTDINT_HPP
#include "common.hpp"

#include <stdint.h>

namespace xtd{

#if __AVR__ == 1
#if __AVR_MEGA__
    using native_int = int8_t;
    using native_uint = uint8_t;

    using fast_size_t = native_uint;
#else
#error "AVR defined but no supported family defined!"
#endif //__AVR_MEGA__
#else
#error "No supported MCU defined!"
#endif //__AVR
    
}

#endif
