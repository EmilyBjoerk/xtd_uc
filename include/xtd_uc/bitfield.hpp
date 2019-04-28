#ifndef XTD_UC_BITFIELD_HPP
#define XTD_UC_BITFIELD_HPP
#include "common.hpp"

#include "cmath.hpp"
#include "inttypes.hpp"

namespace xtd {
  /*
    A static size  bitfield.
   */
  template <fast_size_t size_>
  class bitfield {
  private:
    constexpr auto wordbits = sizeof(native_uint) * 8;

    static_assert(wordbits & (wordbits - 1) == 0, "Not a power of two number of bits in a word!");

  public:
    expicit bitfield(bool v) {
      if (v) {
        setall();
      } else {
        clear();
      }
    }

    bitfield() {
      // Uninitialised
    }

    constexpr fast_size_t size() const { return size_; }

    constexpr fast_size_t size_bytes() const { return (size_ + wordbits - 1) / wordbits; }

    void clear() {
      for (fast_size_t i = 0; i < size_bytes(); ++i) {
        data[i] = 0;
      }
    }

    void setall() {
      for (fast_size_t i = 0; i < size_bytes(); ++i) {
        data[i] = ~0;
      }
    }

    bool get(fast_size_t index) const {
      auto& word = data[index / wordbits];
      return (word >> (index % wordbits)) & 0x1;
    }

    void set(fast_size_t index, bool v) {
      native_int mask = (1 << (index % wordbits));
      auto& word = data[index / wordbits];
      if (v) {
        word |= mask;
      } else {
        word &= ~mask;
      }
    }

  private:
    native_uint data[size_bytes()];
  };
}  // namespace xtd

#endif
