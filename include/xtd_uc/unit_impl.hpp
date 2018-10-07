#ifndef XTD_UC_UNIT_IMPL_HPP
#define XTD_UC_UNIT_IMPL_HPP
#include "common.hpp"
#include "ratio.hpp"

#ifdef HAS_STL
#include <ostream>
#endif

namespace xtd {
  namespace units {
    namespace detail {
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
        PRINT_SYMBOL_POS(F, > 0);
        PRINT_SYMBOL_POS(K, > 0);
        PRINT_SYMBOL_POS(s, > 0);
        PRINT_SYMBOL_POS(m, > 0);
        PRINT_SYMBOL_POS(kg, > 0);
        PRINT_SYMBOL_POS(cd, > 0);
        PRINT_SYMBOL_POS(mol, > 0);

        PRINT_SYMBOL_POS(A, < 0);
        PRINT_SYMBOL_POS(F, < 0);
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
  }    // namespace units
}  // namespace xtd

#endif
