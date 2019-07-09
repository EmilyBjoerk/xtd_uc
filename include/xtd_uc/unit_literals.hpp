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
  constexpr auto operator"" _f##LIT(unsigned long long int x) { \
    using type = units::quantity<TYPE, UNIT, femto>;             \
    return type(static_cast<type::value_type>(x));              \
  }                                                             \
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
    MAKE_LITERAL(ppt, units::unity, long, milli)

    MAKE_LITERAL(g, units::kilogram, long, milli)
    MAKE_LITERAL(kg, units::kilogram, long, ratio<1>)
    MAKE_LITERAL(ton, units::kilogram, long, kilo)

    MAKE_LITERAL(km_h, units::speed, long, ratio<1>)
    MAKE_LITERAL(m_s2, units::acceleration, long, ratio<1>)

    MAKE_LITERAL_PREFIXES(s, units::second, long)
    MAKE_LITERAL(min, units::second, long, ratio<60L>)
    MAKE_LITERAL(h, units::second, long, ratio<60L * 60L>)
    MAKE_LITERAL(days, units::second, long, ratio<60L * 60L * 24L>)

    MAKE_LITERAL_PREFIXES(K, units::kelvin, long)

    MAKE_LITERAL_PREFIXES(m, units::metre, long)
    MAKE_LITERAL_PREFIXES(N, units::newton, long)
    MAKE_LITERAL_PREFIXES(Pa, units::pascal, long)
    MAKE_LITERAL_PREFIXES(J, units::joule, long)
    MAKE_LITERAL_PREFIXES(W, units::watt, long)

    MAKE_LITERAL_PREFIXES(Hz, units::hertz, long)
    MAKE_LITERAL_PREFIXES(A, units::ampere, long)
    MAKE_LITERAL_PREFIXES(V, units::volt, long)
    MAKE_LITERAL_PREFIXES(Ohm, units::ohm, long)
    MAKE_LITERAL_PREFIXES(F, units::farad, long)
    MAKE_LITERAL_PREFIXES(H, units::henry, long)
    MAKE_LITERAL_PREFIXES(C, units::coulomb, long)

#undef MAKE_LITERAL
#undef MAKE_LITERAL_PREFIXES
  }  // namespace unit_literals
}  // namespace xtd

#endif
