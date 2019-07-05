#ifndef XTD_UC_UNITS_HPP
#define XTD_UC_UNITS_HPP
#include "cstdint.hpp"
#include "unit_impl.hpp"

namespace xtd {
  namespace units {
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

    template <typename Value, typename Units, typename Scale>
    class quantity {
    public:
      using units = Units;
      using scale = Scale;
      using value_type = Value;

      constexpr quantity() = default;
      constexpr quantity(value_type val) : v(val){};
      constexpr quantity(const quantity&) = default;
      constexpr quantity(quantity&&) = default;
      template <typename V, typename S>
      constexpr quantity(const quantity<V, units, S>& q)
          : v(static_cast<value_type>(
                xtd::ratio_scale<ratio_t<scale::den * S::num, scale::num * S::den>>(q.count()))) {
        // a * b / c = d * e / f   <=>    a = d * (ce/bf)
      }

      constexpr quantity& operator=(const quantity&) = default;

#ifdef HAS_STL
      friend std::ostream& operator<<(std::ostream& os, const quantity& q) {
        os << xtd::ratio_scale<scale>(static_cast<double>(q.v));
        if (!is_same<unity, units>::value) {
          os << ' ' << units();
        }
        return os;
      }
#endif
      constexpr quantity operator-() const { return quantity(-count()); }

      constexpr quantity& operator++() {
        ++v;
        return *this;
      }

      constexpr quantity operator++(int) {
        auto copy = *this;
        ++v;
        return copy;
      }

      constexpr quantity& operator--() {
        --v;
        return *this;
      }

      constexpr quantity operator--(int) {
        auto copy = *this;
        --v;
        return copy;
      }

      constexpr value_type count() const { return v; }

    private:
      value_type v;
    };

    template <typename v, typename u, typename s, long long x>
    constexpr auto make_unity_valued() {
      return quantity<v, u, ratio_multiply<s, ratio<x>>>(1);
    }

    // Create a new quantity type such that the given quantity is represented with count()==1.
    template <long long x, typename q>
    constexpr auto make_unity_valued(q) {
      return make_unity_valued<typename q::value_type, typename q::units, typename q::scale, x>();
    }

    template <typename val_l, typename val_r, typename units, typename scales_l, typename scales_r>
    constexpr auto operator+(const quantity<val_l, units, scales_l>& lhs,
                             const quantity<val_r, units, scales_r>& rhs) {
      // We have:
      // x * a/b + y * c/d = (xda + ybc)/(bd)
      // let:
      // g_da = gcd(d, a)
      // g_bc = gcd(b, c)
      // g = gcd(da, bc)
      // then:
      // x * a/b + y * c/d = (xda/g + ybc/g) * g/(bd);
      // I.e. the resulting period is g/bd.

      constexpr auto a = scales_l::num;
      constexpr auto b = scales_l::den;
      constexpr auto c = scales_r::num;
      constexpr auto d = scales_r::den;

      constexpr auto g_ad = gcd(a, d);
      constexpr auto g_bc = gcd(b, c);

      constexpr auto a_p = a / g_ad;
      constexpr auto b_p = b / g_bc;
      constexpr auto c_p = c / g_bc;
      constexpr auto d_p = d / g_ad;

      // Note: a_p and d_p are now co-prime, same for b_p and c_p. This means that
      // g_p = gcd(a_p*d_p, b_p*c_p) = gcd(a_p,b_p*c_p)*gcd(d_p,b_p*c_p)
      //     = gcd(a_p, b_p) * gcd(a_p, c_p) * gcd(d_p, b_p) * gcd(d_p, c_p)
      //     = / note that a,b are co-prime and thus a_p and b_p must also be, same for c,d /
      //     = gcd(a_p, c_p) * gcd(d_p, b_p)

      constexpr auto scale_left_p = (a_p / gcd(a_p, c_p)) * (d_p / gcd(d_p, b_p)) * g_ad * g_ad;
      constexpr auto scale_right_p = (b_p / gcd(d_p, b_p)) * (c_p / gcd(a_p, c_p)) * g_bc * g_bc;

      constexpr auto g_s = gcd(scale_left_p, scale_right_p);

      constexpr auto scale_left = scale_left_p / g_s;
      constexpr auto scale_right = scale_right_p / g_s;

      // Scale = g_s * gcd(a_p, c_p) * gcd(d_p, b_p) / (b*d)
      //       = g_s * gcd(a_p, c_p) * gcd(d_p, b_p) / (b_p*d_p*g_ad*g_bc)

      using scale = ratio_multiply<ratio_multiply<ratio_t<gcd(d_p, b_p), max(b_p, d_p)>,
                                                  ratio_t<gcd(a_p, c_p), min(b_p, d_p)>>,
                                   ratio_t<g_s, g_ad * g_bc>>;

      auto ans = lhs.count() * scale_left + rhs.count() * scale_right;
      return quantity<decltype(ans), units, scale>(ans);
    }

    template <typename val_l, typename val_r, typename units, typename scales_l, typename scales_r>
    constexpr auto operator-(const quantity<val_l, units, scales_l>& lhs,
                             const quantity<val_r, units, scales_r>& rhs) {
      return lhs + (-rhs);
    }

    template <typename val_l, typename units_l, typename scales_l, typename val_r, typename units_r,
              typename scales_r>
    constexpr auto operator*(const quantity<val_l, units_l, scales_l>& lhs,
                             const quantity<val_r, units_r, scales_r>& rhs) {
      return quantity<decltype(lhs.count() * rhs.count()),
                      detail::unit_ratios_add<units_l, units_r>,
                      ratio_multiply<scales_l, scales_r>>(lhs.count() * rhs.count());
    }

    template <typename val_l, typename val_r, typename units_r, typename scales_r>
    constexpr auto operator*(val_l lhs, const quantity<val_r, units_r, scales_r>& rhs) {
      auto ans = lhs * rhs.count();
      return quantity<decltype(ans), units_r, scales_r>(ans);
    }

    template <typename val_l, typename units_l, typename scales_l, typename val_r>
    constexpr auto operator*(const quantity<val_l, units_l, scales_l>& lhs, val_r rhs) {
      return rhs * lhs;
    }

    template <typename val_l, typename units_l, typename scales_l, typename val_r, typename units_r,
              typename scales_r>
    constexpr auto operator/(const quantity<val_l, units_l, scales_l>& lhs,
                             const quantity<val_r, units_r, scales_r>& rhs) {
      return quantity<decltype(lhs.count() / rhs.count()),
                      detail::unit_ratios_subtract<units_l, units_r>,
                      ratio_divide<scales_l, scales_r>>(lhs.count() / rhs.count());
    }

    template <typename val_l, typename val_r, typename units_r, typename scales_r>
    constexpr auto operator/(const val_l lhs, const quantity<val_r, units_r, scales_r>& rhs) {
      using units = detail::unit_ratios_subtract<unity, units_r>;
      return quantity<val_l, units, ratio<1, scales_r::num>>(lhs * scales_r::den / rhs.count());
    }

    template <typename val_l, typename units_l, typename scales_l, typename val_r>
    constexpr auto operator/(const quantity<val_l, units_l, scales_l>& lhs, const val_r rhs) {
      return lhs / quantity<val_r, unity, ratio<1>>(rhs);
    }

    template <typename val_l, typename val_r, typename units, typename scales_l, typename scales_r>
    constexpr auto operator==(const quantity<val_l, units, scales_l>& lhs,
                              const quantity<val_r, units, scales_r>& rhs) {
      return 0 == (lhs - rhs).count();
    }

    template <typename val_l, typename val_r, typename units, typename scales_l, typename scales_r>
    constexpr auto operator<(const quantity<val_l, units, scales_l>& lhs,
                             const quantity<val_r, units, scales_r>& rhs) {
      // We have:
      // x*a/b < y*c/d <=> x*a*d < y*c*b
      // let:
      // g = gcd(a*d, c*b)
      // then:
      // x*(a*d)/g < y*(c*b)/g

      // This is a compile time mult. compiler should warn of overflow
      constexpr auto da = scales_r::den * scales_l::num;
      constexpr auto bc = scales_l::den * scales_r::num;
      constexpr auto g = gcd(da, bc);

      // FIXME: Compute without overflow
      return lhs.count() * (da / g) < rhs.count() * (bc / g);
    }

    template <typename val_l, typename val_r, typename units, typename scales_l, typename scales_r>
    constexpr auto operator>(const quantity<val_l, units, scales_l>& lhs,
                             const quantity<val_r, units, scales_r>& rhs) {
      return rhs < lhs;
    }

    template <typename val_l, typename val_r, typename units, typename scales_l, typename scales_r>
    constexpr auto operator<=(const quantity<val_l, units, scales_l>& lhs,
                              const quantity<val_r, units, scales_r>& rhs) {
      return !(rhs < lhs);
    }

    template <typename val_l, typename val_r, typename units, typename scales_l, typename scales_r>
    constexpr auto operator>=(const quantity<val_l, units, scales_l>& lhs,
                              const quantity<val_r, units, scales_r>& rhs) {
      return !(lhs < rhs);
    }

    template <typename val_l, typename val_r, typename units, typename scales_l, typename scales_r>
    constexpr auto operator!=(const quantity<val_l, units, scales_l>& lhs,
                              const quantity<val_r, units, scales_r>& rhs) {
      return !(lhs == rhs);
    }

    using namespace si_prefixes;

    // Default quantities
    using kg = quantity<int, kilogram, ratio<1>>;
    using km = quantity<int, metre, kilo>;
    using force = quantity<int, newton, ratio<1>>;

    template <typename type, typename scale>
    using mass = quantity<type, kilogram, scale>;

    template <typename type, typename scale>
    using capacitance = quantity<type, farad, scale>;

    template <typename type, typename scale>
    using temperature = quantity<type, kelvin, scale>;

    template <typename type, typename scale>
    using current = quantity<type, ampere, scale>;

    template <typename type, typename scale>
    using voltage = quantity<type, volt, scale>;

    template <typename type, typename scale>
    using time = quantity<type, second, scale>;

    template <typename type, typename scale>
    using frequency = quantity<type, hertz, scale>;

    template <typename type, typename scalee>
    using scale = quantity<type, unity, scalee>;

  }  // namespace units
}  // namespace xtd

#include "unit_literals.hpp"

#endif
