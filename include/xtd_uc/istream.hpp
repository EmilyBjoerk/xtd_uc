#ifndef XTD_UC_ISTREAM_HPP
#define XTD_UC_ISTREAM_HPP
#include "common.hpp"

#include "algorithm.hpp"

#if 0 // Not done/working yet

namespace xtd {

  namespace ios_base {
    using iostate = uint8_t;

    static constexpr iostate goodbit = 0;
    static constexpr iostate badbit = 1;
    static constexpr iostate failbit = 2;
    static constexpr iostate eofbit = 4;
  }

  template <typename Stream>
  class istream;

  template <>
  class istream<usart> {
  public:
    using iostate = ios_base::iostate;

    bool good() const { rdstate() == xtd::ios_base::goodbit; }
    bool fail() const { rdstate() & (xtd::ios_base::failbit | xtd::ios_base::badbit); }
    bool eof() const { rdstate() & xtd::ios_base::eofbit; }
    bool bad() const { rdstate() & xtd::ios_base::badbit; }

    bool operator!() const { return fail(); }
    explicit operator bool() const { return !fail(); }

    void clear(iostate state = xtd::ios_base::goodbit) { m_state = state; }

    iostate rdstate() const { return m_state; }
    void setstate(iostate state) { clear(rdstate() | state); }

    friend auto operator>>(istream<usart>& is, char& c) {}

    template <typename T, typename xtd::enable_if_t<xtd::numeric_limits<T>::is_integer>>
    friend auto operator>>(istream<usart>& is, T& integer) {
      using unsigned_type = unsigned T;
      using limits = xtd::numeric_limits<T>;
      using unsigned_limits = xtd::numeric_limits<unsigned_type>;

      is.skipws();
      if (!is.good()) {
        return;
      }

      static constexpr auto zero = unsigned_type();
      static constexpr auto mulmask8 = ~((~zero) >> 3);  // Top three bits set.

      auto absvalue = zero;
      auto negative = (is.peek() == '-');
      auto overflow = false;
      if (negative) {
        is.get();  // Remove it from the buffer.
        if (!limits::is_signed) {
          while (is.good() && !is.next_is_ws()) {
            is.get();
          }
          is.setstate(xtd::ios_base::failbit);
          integer = limits::min();
        }
      }

      while (!is.next_is_ws()) {
        auto c = is.get();
        if (is.good() && !overflow) {
          if (!isdigit(c)) {
            setstate(xtd::ios_base::failbit);
            continue;
          }

          // newabsvalue = 10*absvalue + (c-'0') = absvalue*(8+2) + (c-'0')
          //             = (absvalue << 3) + (absvalue << 1) + (c-'0')
          auto by8 = absvalue << 3;
          auto by2 = absvalue << 1;
          auto digit = c - '0';

          // Check that:
          // 1) "absvalue*8" doesn't overflow.
          // 2) "(absvalue*8) + (absvalue*2 + digit)" doesn't overflow
          // Note: "absvalue*2 + digit" will never overflow as "absvalue*8" is representable and
          // "absvalue*2 + 9 < absvalue*8" when "3/2 < absvalue" so the only absvalue for which
          // "absvalue*2 + digit" could overflow when "absvalue*8" didn't overflow is when "absvalue
          // <= 3/2" but we know that there exists no integer type for which "3/2" is above the
          // maximum representable value.
          if (absvalue & mulmask8 != 0 || by2 + digit > unsigned_limit::max() - by8) {
            overflow = true;
            continue;
          }
          absvalue = by8 + by2 + digit;
        }
      }

      if (is.bad()) {
        return is;
      }

      if (overflow) {
        integer = negative ? limits::min() : limits::max();
        setstate(xtd::ios_base::failbit);
        return is;
      }

      if (limits::is_signed) {
        if (negative) {
          // Remember that "-limits::min()" is not representable in 2s complement.
          auto max_abs = static_cast<unsigned_type>(-(limits::min() + 1)) + 1;
          if (absvalue > max_abs) {
            integer = limits::min();
            setstate(xtd::ios_base::failbit);
          } else {
            integer = -absvalue;
          }
        } else {
          auto max_abs = static_cast<unsigned_type>(limits::max());
          if (absvalue > max_abs) {
            integer = limits::max();
            setstate(xtd::ios_base::failbit);
          } else {
            integer = absvalue;
          }
        }
      } else {  // if (limits::is_signed)
        integer = absvalue;
      }
      return is;
    }

  private:
    iostate m_state = xtd::ios_base::goodbit;

    bool next_is_ws() const {}

    void skipws() {}

    char get() {
      // TODO: Add timeout
      while (usart::is_enabled() && !usart::has_char() &&
             rx_status_flag::good == usart::rx_status()) {
        // wait until we get a char
      }

      if (usart::has_char()) {
        return usart::get();
      }
      if (!usart::is_enabled()) {
        setstate(xtd::ios_base::eofbit);
      }
      if (rx_status_flag::good != usart::rx_status()) {
        setstate(xtd::ios_base::badbit);
      }
      return 0;
    }
  };
}

#endif

#endif
