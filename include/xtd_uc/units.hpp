#ifndef XTD_UC_UNITS_HPP
#define XTD_UC_UNITS_HPP
#include "common.hpp"
#include "cstdint.hpp"
#include "ratio.hpp"

#ifdef HAS_STL
#include <ostream>
#endif

namespace xtd {
  namespace units {
    namespace detail {
      enum class base_units : uint8_t { ampere, kelvin, second, metre, kilograms, candela, mole };

      template <typename A, typename K, typename s, typename m, typename kg, typename cd,
                typename mol>
      struct unit_ratios {
        using ampere = A;
        using kelvin = K;
        using second = s;
        using metre = m;
        using kilogram = kg;
        using candela = cd;
        using mole = mol;

        explicit unit_ratios() = default;
      };

#ifdef HAS_STL
      template <typename A, typename K, typename s, typename m, typename kg, typename cd,
                typename mol>
      std::ostream& operator<<(std::ostream& os, unit_ratios<A, K, s, m, kg, cd, mol>) {
#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)
        bool first = true;
#define PRINT_SYMBOL_POS(SYMBOL, CMP)                           \
  do {                                                          \
    if (SYMBOL::num CMP) {                                      \
      if (!first) {                                             \
        os << "*";                                              \
      }                                                         \
      os << STRINGIFY(SYMBOL);                                  \
      first = false;                                            \
      if (SYMBOL::den == 1) {                                   \
        if (SYMBOL::num != 1) {                                 \
          os << "^" << SYMBOL::num;                             \
        }                                                       \
      } else {                                                  \
        os << "^(" << SYMBOL::num << "/" << SYMBOL::den << ")"; \
      }                                                         \
    }                                                           \
  } while (0)

        PRINT_SYMBOL_POS(A, > 0);
        PRINT_SYMBOL_POS(K, > 0);
        PRINT_SYMBOL_POS(s, > 0);
        PRINT_SYMBOL_POS(m, > 0);
        PRINT_SYMBOL_POS(kg, > 0);
        PRINT_SYMBOL_POS(cd, > 0);
        PRINT_SYMBOL_POS(mol, > 0);

        PRINT_SYMBOL_POS(A, < 0);
        PRINT_SYMBOL_POS(K, < 0);
        PRINT_SYMBOL_POS(s, < 0);
        PRINT_SYMBOL_POS(m, < 0);
        PRINT_SYMBOL_POS(kg, < 0);
        PRINT_SYMBOL_POS(cd, < 0);
        PRINT_SYMBOL_POS(mol, < 0);

#undef PRINT_SYMBOL
#undef STRINGIFY2
#undef STRINGIFY

        return os;
      }

#endif

      template <typename lhs, typename rhs>
      using unit_ratios_add = unit_ratios<ratio_add<typename lhs::ampere, typename rhs::ampere>,
                                          ratio_add<typename lhs::kelvin, typename rhs::kelvin>,
                                          ratio_add<typename lhs::second, typename rhs::second>,
                                          ratio_add<typename lhs::metre, typename rhs::metre>,
                                          ratio_add<typename lhs::kilogram, typename rhs::kilogram>,
                                          ratio_add<typename lhs::candela, typename rhs::candela>,
                                          ratio_add<typename lhs::mole, typename rhs::mole>>;

      template <typename lhs, typename rhs>
      using unit_ratios_subtract =
          unit_ratios<ratio_subtract<typename lhs::ampere, typename rhs::ampere>,
                      ratio_subtract<typename lhs::kelvin, typename rhs::kelvin>,
                      ratio_subtract<typename lhs::second, typename rhs::second>,
                      ratio_subtract<typename lhs::metre, typename rhs::metre>,
                      ratio_subtract<typename lhs::kilogram, typename rhs::kilogram>,
                      ratio_subtract<typename lhs::candela, typename rhs::candela>,
                      ratio_subtract<typename lhs::mole, typename rhs::mole>>;

      using _ = ratio<0>;

      using ampere_impl = unit_ratios<ratio<1>, _, _, _, _, _, _>;
      using kelvin_impl = unit_ratios<_, ratio<1>, _, _, _, _, _>;
      using second_impl = unit_ratios<_, _, ratio<1>, _, _, _, _>;
      using metre_impl = unit_ratios<_, _, _, ratio<1>, _, _, _>;
      using kilogram_impl = unit_ratios<_, _, _, _, ratio<1>, _, _>;
      using candela_impl = unit_ratios<_, _, _, _, _, ratio<1>, _>;
      using mole_impl = unit_ratios<_, _, _, _, _, _, ratio<1>>;
      using unity_impl = unit_ratios<_, _, _, _, _, _, _>;
    }  // namespace detail

    // The "special" unity unit
    using unity = detail::unity_impl;

    // The seven base units
    using ampere = detail::ampere_impl;
    using kelvin = detail::kelvin_impl;
    using second = detail::second_impl;
    using metre = detail::metre_impl;
    using kilogram = detail::kilogram_impl;
    using candela = detail::candela_impl;
    using mole = detail::mole_impl;

    // Some named units that make the below definitions easier
    using area = detail::unit_ratios_add<metre, metre>;
    using volume = detail::unit_ratios_add<area, metre>;
    using speed = detail::unit_ratios_subtract<metre, second>;
    using acceleration = detail::unit_ratios_subtract<speed, second>;

    // The 22 named derived units
    using hertz = detail::unit_ratios_subtract<unity, second>;
    using radian = unity;
    using steradian = unity;
    using newton = detail::unit_ratios_add<kilogram, acceleration>;
    using pascal = detail::unit_ratios_subtract<newton, area>;
    using joule = detail::unit_ratios_add<newton, metre>;
    using watt = detail::unit_ratios_subtract<joule, second>;
    using coulomb = detail::unit_ratios_add<ampere, second>;
    using volt = detail::unit_ratios_subtract<joule, coulomb>;
    using farad = detail::unit_ratios_subtract<coulomb, volt>;
    using ohm = detail::unit_ratios_subtract<volt, ampere>;
    using siemens = detail::unit_ratios_subtract<ampere, volt>;
    using weber = detail::unit_ratios_subtract<joule, ampere>;
    using tesla = detail::unit_ratios_subtract<weber, area>;
    using henry = detail::unit_ratios_subtract<ohm, second>;
    using celsius = kelvin;
    using lumen = candela;
    using lux = detail::unit_ratios_subtract<candela, area>;
    using becquerel = hertz;
    using gray = detail::unit_ratios_subtract<joule, kilogram>;
    using sievert = gray;
    using katal = detail::unit_ratios_subtract<mole, second>;

#ifdef HAS_STL
    inline std::ostream& operator<<(std::ostream& os, farad) { return os << "F"; }
#endif

    template <typename value, typename units, typename scales>
    class quantity {
    public:
      using unit = units;
      using scale = scales;
      using value_type = value;

      constexpr quantity(value_type val) : v(val){};

      template <typename V, typename S>
      constexpr quantity(const quantity<V, units, S>& q) {
        // a * b / c = d * e / f   <=>    a = d * (ce/bf)
        constexpr auto bf = scales::num * S::den;
        constexpr auto ce = scales::den * S::num;
        v = static_cast<value_type>(xtd::ratio_scale<ratio_t<ce, bf>>(q.v));
      }

#ifdef HAS_STL
      friend std::ostream& operator<<(std::ostream& os, const quantity& q) {
        os << xtd::ratio_scale<scales>(static_cast<double>(q.v));
        if (!is_same<unity, units>::value) {
          os << ' ' << units();
        }
        return os;
      }
#endif

      value_type v;
    };

    namespace detail {
      template <typename val, typename units, typename scales_l, typename scales_r>
      constexpr auto cmp(const quantity<val, units, scales_l>& lhs,
                         const quantity<val, units, scales_r>& rhs) {
        // a * b / c = d * e / f
        // a * (bf/gcd(bf,ce)) = d * (ce/(gcd(bf,ce))

        // This is a compile time mult. compiler should warn of overflow
        constexpr auto bf = scales_l::num * scales_r::den;
        constexpr auto ce = scales_l::den * scales_r::num;
        constexpr auto g = gcd(bf, ce);
        return lhs.v * (bf / g) - rhs.v * (ce / g);
      }
    }  // namespace detail

    template <typename val, typename units, typename scales_l, typename scales_r>
    constexpr auto operator==(const quantity<val, units, scales_l>& lhs,
                              const quantity<val, units, scales_r>& rhs) {
      return 0 == cmp(lhs, rhs);
    }

    template <typename val, typename units, typename scales_l, typename scales_r>
    constexpr auto operator<(const quantity<val, units, scales_l>& lhs,
                             const quantity<val, units, scales_r>& rhs) {
      return cmp(lhs, rhs) < 0;
    }

    template <typename val, typename units, typename scales_l, typename scales_r>
    constexpr auto operator>(const quantity<val, units, scales_l>& lhs,
                             const quantity<val, units, scales_r>& rhs) {
      return rhs < lhs;
    }

    template <typename val, typename units, typename scales_l, typename scales_r>
    constexpr auto operator<=(const quantity<val, units, scales_l>& lhs,
                              const quantity<val, units, scales_r>& rhs) {
      return !(rhs < lhs);
    }

    template <typename val, typename units, typename scales_l, typename scales_r>
    constexpr auto operator>=(const quantity<val, units, scales_l>& lhs,
                              const quantity<val, units, scales_r>& rhs) {
      return !(lhs < rhs);
    }

    template <typename val, typename units, typename scales_l, typename scales_r>
    constexpr auto operator!=(const quantity<val, units, scales_l>& lhs,
                              const quantity<val, units, scales_r>& rhs) {
      return !(lhs == rhs);
    }

    template <typename val_l, typename units_l, typename scales_l, typename val_r, typename units_r,
              typename scales_r>
    constexpr auto operator*(const quantity<val_l, units_l, scales_l>& lhs,
                             const quantity<val_r, units_r, scales_r>& rhs) {
      return quantity<decltype(lhs.v * rhs.v),                    // value
                      detail::unit_ratios_add<units_l, units_r>,  // unit
                      ratio_multiply<scales_l, scales_r>          // scale
                      >(lhs.v * rhs.v);
    }

    template <typename val_l, typename units_l, typename scales_l, typename val_r, typename units_r,
              typename scales_r>
    constexpr auto operator/(const quantity<val_l, units_l, scales_l>& lhs,
                             const quantity<val_r, units_r, scales_r>& rhs) {
      return quantity<decltype(lhs.v / rhs.v), detail::unit_ratios_subtract<units_l, units_r>,
                      ratio_divide<scales_l, scales_r>>(lhs.v / rhs.v);
    }

    using kg = quantity<int, units::kilogram, ratio<1>>;
    using km = quantity<int, units::metre, kilo>;
    using force = quantity<int, units::newton, ratio<1>>;

  }  // namespace units

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

    MAKE_LITERAL(g, units::kilogram, int, milli)
    MAKE_LITERAL(kg, units::kilogram, int, ratio<1>)
    MAKE_LITERAL(ton, units::kilogram, int, kilo)

    MAKE_LITERAL(km_h, units::speed, int, ratio<1>)
    MAKE_LITERAL(m_s2, units::acceleration, int, ratio<1>)

    MAKE_LITERAL(s, units::second, int, ratio<1>)

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

  }  // namespace unit_literals
}  // namespace xtd
#endif
