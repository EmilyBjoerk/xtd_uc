#ifndef XTD_UC_UNIT_LITERALS_HPP
#define XTD_UC_UNIT_LITERALS_HPP
#include "common.hpp"
#include "units.hpp"

namespace xtd {
  namespace unit_literals {

#define MAKE_LITERAL(LIT, UNIT, TYPE, SCALE)                   \
  constexpr auto operator"" _##LIT(unsigned long long int x) { \
    using type = units::quantity<TYPE, UNIT, SCALE>;           \
    return type(static_cast<type::value_type>(x));             \
  }

#define MAKE_LITERAL_PREFIXES(LIT, UNIT, TYPE)                  \
  constexpr auto operator"" _p##LIT(unsigned long long int x) { \
    using type = units::quantity<TYPE, UNIT, pico>;             \
    return type(static_cast<type::value_type>(x));              \
  }                                                             \
  constexpr auto operator"" _n##LIT(unsigned long long int x) { \
    using type = units::quantity<TYPE, UNIT, nano>;             \
    return type(static_cast<type::value_type>(x));              \
  }                                                             \
  constexpr auto operator"" _u##LIT(unsigned long long int x) { \
    using type = units::quantity<TYPE, UNIT, micro>;            \
    return type(static_cast<type::value_type>(x));              \
  }                                                             \
  constexpr auto operator"" _m##LIT(unsigned long long int x) { \
    using type = units::quantity<TYPE, UNIT, milli>;            \
    return type(static_cast<type::value_type>(x));              \
  }                                                             \
  constexpr auto operator"" _##LIT(unsigned long long int x) {  \
    using type = units::quantity<TYPE, UNIT, ratio<1>>;         \
    return type(static_cast<type::value_type>(x));              \
  }                                                             \
  constexpr auto operator"" _k##LIT(unsigned long long int x) { \
    using type = units::quantity<TYPE, UNIT, kilo>;             \
    return type(static_cast<type::value_type>(x));              \
  }                                                             \
  constexpr auto operator"" _M##LIT(unsigned long long int x) { \
    using type = units::quantity<TYPE, UNIT, mega>;             \
    return type(static_cast<type::value_type>(x));              \
  }                                                             \
  constexpr auto operator"" _G##LIT(unsigned long long int x) { \
    using type = units::quantity<TYPE, UNIT, giga>;             \
    return type(static_cast<type::value_type>(x));              \
  }

    MAKE_LITERAL(ppm, units::unity, long, micro)

    MAKE_LITERAL(g, units::kilogram, int, milli)
    MAKE_LITERAL(kg, units::kilogram, int, ratio<1>)
    MAKE_LITERAL(ton, units::kilogram, int, kilo)

    MAKE_LITERAL(km_h, units::speed, int, ratio<1>)
    MAKE_LITERAL(m_s2, units::acceleration, int, ratio<1>)

    MAKE_LITERAL_PREFIXES(s, units::second, int)
    MAKE_LITERAL(min, units::second, int, ratio<60L>)
    MAKE_LITERAL(h, units::second, int, ratio<60L * 60L>)
    MAKE_LITERAL(days, units::second, int, ratio<60L * 60L * 24L>)

    MAKE_LITERAL_PREFIXES(K, units::kelvin, int)

    MAKE_LITERAL_PREFIXES(m, units::metre, int)
    MAKE_LITERAL_PREFIXES(N, units::newton, int)
    MAKE_LITERAL_PREFIXES(Pa, units::pascal, int)
    MAKE_LITERAL_PREFIXES(J, units::joule, int)
    MAKE_LITERAL_PREFIXES(W, units::watt, int)

    MAKE_LITERAL_PREFIXES(Hz, units::hertz, int)
    MAKE_LITERAL_PREFIXES(A, units::ampere, int)
    MAKE_LITERAL_PREFIXES(V, units::volt, int)
    MAKE_LITERAL_PREFIXES(Ohm, units::ohm, int)
    MAKE_LITERAL_PREFIXES(F, units::farad, int)
    MAKE_LITERAL_PREFIXES(H, units::henry, int)
    MAKE_LITERAL_PREFIXES(C, units::coulomb, int)

#undef MAKE_LITERAL
#undef MAKE_LITERAL_PREFIXES
  }  // namespace unit_literals
}  // namespace xtd

#endif
