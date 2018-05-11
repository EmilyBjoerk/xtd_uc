#ifndef XTD_UC_UTILITY_HPP
#define XTD_UC_UTILITY_HPP
#include "common.hpp"

#include <stdint.h>
#include "limits.hpp"
#include "type_traits.hpp"

namespace xtd {
  template <typename T>
  constexpr void swap(T& a, T& b) {
    T c{a};
    a = b;
    b = c;
  }

  inline void set_bit(volatile uint8_t& sfr, int bit) {
    sfr = static_cast<uint8_t>(sfr | (1 << bit));
  }

  inline void clr_bit(volatile uint8_t& sfr, int bit) {
    sfr = static_cast<uint8_t>(sfr & ~(1 << bit));
  }

  // http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2009/n2951.html
  template <class T, class U,
            class = typename enable_if<
                (is_lvalue_reference<T>::value ? is_lvalue_reference<U>::value : true)
                // TODO: Add this back once we have is_convertible
                /*&&
                is_convertible<typename remove_reference<U>::type*,
                typename remove_reference<T>::type*>::value*/>::type>
  inline T&& forward(U&& u) {
    return static_cast<T&&>(u);
  }

  template <class T>
  constexpr typename remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(t);
  }

  template <class T>
  typename add_rvalue_reference<T>::type declval() noexcept;
}  // namespace xtd

#endif
