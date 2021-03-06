#ifndef XTD_UC_CHRONO_NOCLOCK_HPP
#define XTD_UC_CHRONO_NOCLOCK_HPP
#include "common.hpp"

#include "cstdint.hpp"
#include "units.hpp"

#ifdef HAS_STL
#include <iomanip>
#include <ostream>
#endif

// Namespace "chrono_literals" is not provided due to frequent conflict
// with xtd::unit_literals which you sould use instead.

namespace xtd {
  namespace chrono {
    template <class Rep, class Period = xtd::ratio<1>>
    using duration = units::quantity<Rep, units::second, Period>;

    // Non-compliance, minimum type requirements not met.
    using nanoseconds = duration<int32_t, nano>;    // +- 2.1 seconds
    using microseconds = duration<int32_t, micro>;  // +- 35.8 minutes
    using milliseconds = duration<int32_t, milli>;  // +- 24.9 days
    using seconds = duration<int16_t>;              // +- 9.1 hours
    using minutes = duration<int16_t, ratio<60>>;   // +- 22.7 days
    using hours = duration<int16_t, ratio<3600>>;   // +- 3.7 years

    // Non-standard extensions
    using days = duration<int16_t, ratio<24L * 3600L>>;  // +- 127 days

    template <class Clock, class Duration = typename Clock::duration>
    class time_point {
    public:
      using clock = Clock;
      using duration = Duration;
      using value_type = typename Duration::value_type;
      using scale = typename Clock::scale;

      constexpr time_point() = default;  // Default time of clock init
      constexpr explicit time_point(const duration& d) : m_d(d) {}

      constexpr duration time_since_epoch() const { return m_d; }

      time_point& operator=(const time_point&) = default;

      bool operator==(const time_point& rhs) const { return m_d == rhs.m_d; };
      bool operator!=(const time_point& rhs) const { return !(*this == rhs); };

      template <typename D>
      time_point& operator=(const time_point<Clock, D>& that) {
        m_d = that.time_since_epoch();
        return (*this);
      }

#ifdef HAS_STL
      friend std::ostream& operator<<(std::ostream& os, const time_point& t) {
        return os << t.time_since_epoch();
      }
#endif

    private:
      duration m_d;
    };

    template <class C1, class D1, class D2>
    constexpr bool operator<(const time_point<C1, D1>& lhs, const time_point<C1, D2>& rhs) {
      return lhs.time_since_epoch() < rhs.time_since_epoch();
    }

    template <class C1, class D1, class D2>
    constexpr bool operator<=(const time_point<C1, D1>& lhs, const time_point<C1, D2>& rhs) {
      return !(rhs < lhs);
    }

    template <class C1, class D1, class C2, class D2>
    constexpr bool operator>(const time_point<C1, D1>& lhs, const time_point<C1, D2>& rhs) {
      return rhs < lhs;
    }

    template <class C1, class D1, class D2>
    constexpr bool operator>=(const time_point<C1, D1>& lhs, const time_point<C1, D2>& rhs) {
      return !(lhs < rhs);
    }

    // -------------------------------------------------------------------------
    // The operator overloads for time_point/duration +- time_point/duration differ
    // from standard c++ in the following ways:
    // 1) time_point - time_point = duration
    // 2) time_point + time_point = nonsensical
    // 3) time_point +- duration = time_point
    // 4) duration +- duration = duration;
    // -------------------------------------------------------------------------
    template <class C, class D1, class D2>
    constexpr auto operator-(const time_point<C, D1>& lhs, const time_point<C, D2>& rhs) {
      return lhs.time_since_epoch() - rhs.time_since_epoch();
    }

    template <class C, class D, class Value_Type, class Scale>
    constexpr auto operator+(const time_point<C, D>& lhs, const duration<Value_Type, Scale>& rhs) {
      auto ans = lhs.time_since_epoch() + rhs;
      return time_point<C, decltype(ans)>(ans);
    }

    template <class C, class D, class Value_Type, class Scale>
    constexpr auto operator-(const time_point<C, D>& lhs, const duration<Value_Type, Scale>& rhs) {
      auto ans = lhs.time_since_epoch() - rhs;
      return time_point<C, decltype(ans)>(ans);
    }

    template <class Value_Type1, class Scale1>
    constexpr auto operator-(const duration<Value_Type1, Scale1>& arg) {
      return duration<Value_Type1, Scale1>(-arg.count());
    }

    template <class Value_Type1, class Scale1, class Value_Type2, class Scale2>
    constexpr auto operator-(const duration<Value_Type1, Scale1>& lhs,
                             const duration<Value_Type2, Scale2>& rhs) {
      return lhs + (-rhs);
    }

    template <class Value_Type1, class Scale1, class Value_Type2, class Scale2>
    constexpr auto operator+(const duration<Value_Type1, Scale1>& lhs,
                             const duration<Value_Type2, Scale2>& rhs) {
      // So we have:
      // x * a/b + y * c/d = (xda + ybc)/(bd)
      // let:
      // g = gcd(da, bc)
      // then:
      // x * a/b + y * c/d = (xda/g + ybc/g) * g/(bd);
      // I.e. the resulting scale is g/bd.
      constexpr auto g = gcd(Scale2::den * Scale1::num, Scale1::den * Scale2::num);
      using new_scale = ratio_t<g, Scale1::den * Scale2::den>;

      return duration<int64_t, new_scale>(duration<int64_t, new_scale>(lhs).count() +
                                          duration<int64_t, new_scale>(rhs).count());
    }
  }  // namespace chrono

  namespace units {
#ifdef HAS_STL
    // This overload must be in the same namespace as xtd::units::quantity of which
    // duration is an alias. Otherwise this overload will not be found through ADL.
    template <typename R, typename P>
    std::ostream& operator<<(std::ostream& os, const chrono::duration<R, P>& d) {
      using namespace chrono;
      int h = ratio_convert<hours::scale, P>(d.count());
      auto rh = d - hours(h);
      int m = ratio_convert<minutes::scale, decltype(rh)::scale>(rh.count());
      auto rm = rh - minutes(m);
      int s = ratio_convert<seconds::scale, decltype(rm)::scale>(rm.count());
      auto rs = rm - seconds(s);
      int ms = ratio_convert<milliseconds::scale, decltype(rs)::scale>(rs.count());

      return os << std::setfill('0') << std::setw(2) << h << ":" << std::setfill('0')
                << std::setw(2) << m << ":" << std::setfill('0') << std::setw(2) << s << "."
                << std::setfill('0') << std::setw(3) << ms;
    }
#endif
  }  // namespace units
}  // namespace xtd

#ifdef HAS_STL
#include <chrono>
namespace std {
  namespace chrono {
    template <typename R, typename P>
    std::ostream& operator<<(std::ostream& os, const std::chrono::duration<R, P>& d) {
      auto h = std::chrono::duration_cast<std::chrono::hours>(d);
      auto rh = d - h;
      auto m = std::chrono::duration_cast<std::chrono::minutes>(rh);
      auto rm = rh - m;
      auto s = std::chrono::duration_cast<std::chrono::seconds>(rm);
      auto rs = rm - seconds(s);
      auto ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(rs);

      return os << std::setfill('0') << std::setw(2) << h.count() << ":" << std::setfill('0')
                << std::setw(2) << m.count() << ":" << std::setfill('0') << std::setw(2)
                << s.count() << "." << std::setfill('0') << std::setw(3) << ms.count();
    }
  }  // namespace chrono
}  // namespace std
#endif

#endif
