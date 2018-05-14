#ifndef XTD_UC_CHRONO_NOCLOCK_HPP
#define XTD_UC_CHRONO_NOCLOCK_HPP
#include "common.hpp"

#include "cstdint.hpp"
#include "ratio.hpp"

#ifdef ENABLE_TEST
#include <iomanip>
#include <ostream>
#endif

namespace xtd {
  namespace chrono {
    template <class Rep, class Period = xtd::ratio<1>>
    class duration {
    public:
      using rep = Rep;
      using period = Period;

      constexpr duration() = default;
      constexpr explicit duration(rep t) : ticks(t) {}

      template <class Rep2, class Period2>
      constexpr duration(const duration<Rep2, Period2>& d)
      : ticks(ratio_convert<period, Period2, decltype(rep()+Rep2())>(d.count())) {}

      duration& operator=(const duration&) = default;

      constexpr rep count() const { return ticks; }

      constexpr duration& operator++() {
        ticks++;
        return *this;
      }

      constexpr duration operator++(int) {
        auto copy = *this;
        ticks++;
        return copy;
      }

      constexpr duration& operator--() {
        ticks--;
        return *this;
      }

      constexpr duration operator--(int) {
        auto copy = *this;
        ticks--;
        return copy;
      }

    private:
      rep ticks = 0;
    };

    template <class Rep1, class Period1, class Rep2, class Period2>
    constexpr auto cmp(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs) {
      // CAUTION: All standard durations are defined with at most 32 bits which means that
      // the below will not overflow as it is computed in 64 bits.
      // WARNING: This may overflow need to come up with something better.
      constexpr auto div = gcd(Period1::num * Period2::den, Period2::num * Period1::den);
      constexpr int64_t lhs_factor = (Period1::num * Period2::den) / div;
      constexpr int64_t rhs_factor = (Period2::num * Period1::den) / div;
      return lhs.count() * lhs_factor - rhs.count() * rhs_factor;
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    constexpr bool operator<(const duration<Rep1, Period1>& lhs,
                             const duration<Rep2, Period2>& rhs) {
      return cmp(lhs, rhs) < 0;
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    constexpr bool operator<=(const duration<Rep1, Period1>& lhs,
                              const duration<Rep2, Period2>& rhs) {
      return !(rhs < lhs);
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    constexpr bool operator>(const duration<Rep1, Period1>& lhs,
                             const duration<Rep2, Period2>& rhs) {
      return rhs < lhs;
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    constexpr bool operator>=(const duration<Rep1, Period1>& lhs,
                              const duration<Rep2, Period2>& rhs) {
      return !(lhs < rhs);
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    constexpr bool operator==(const duration<Rep1, Period1>& lhs,
                              const duration<Rep2, Period2>& rhs) {
      return cmp(lhs, rhs) == 0;
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    constexpr bool operator!=(const duration<Rep1, Period1>& lhs,
                              const duration<Rep2, Period2>& rhs) {
      return !(lhs == rhs);
    }

    // Non-compliance, minimum type requirements not met.
    using nanoseconds = duration<int32_t, nano>;    // +- 2.1 seconds
    using microseconds = duration<int32_t, micro>;  // +- 35.8 minutes
    using milliseconds = duration<int32_t, milli>;  // +- 24.9 days
    using seconds = duration<int16_t>;              // +- 9.1 hours
    using minutes = duration<int16_t, ratio<60>>;   // +- 22.7 days
    using hours = duration<int16_t, ratio<3600>>;   // +- 3.7 years

    // Non-standard extensions
    using days = duration<int16_t, ratio<24L * 3600L>>;  // +- 127 days

#ifdef ENABLE_TEST
    template <typename R, typename P>
    std::ostream& operator<<(std::ostream& os, const duration<R, P>& d) {
      int h = ratio_convert<hours::period, P>(d.count());
      auto rh = d - hours(h);
      int m = ratio_convert<minutes::period, typename decltype(rh)::period>(rh.count());
      auto rm = rh - minutes(m);
      int s = ratio_convert<seconds::period, typename decltype(rm)::period>(rm.count());
      auto rs = rm - seconds(s);
      int ms = ratio_convert<milliseconds::period, typename decltype(rs)::period>(rs.count());

      return os << std::setfill('0') << std::setw(2) << h << ":" << std::setfill('0')
                << std::setw(2) << m << ":" << std::setfill('0') << std::setw(2) << s << "."
                << std::setfill('0') << std::setw(3) << ms;
    }
#endif
  }  // namespace chrono

  namespace chrono_literals {

#define MAKE_LITERAL(TYPE, LIT)                                        \
  constexpr chrono::TYPE operator"" _##LIT(unsigned long long int x) { \
    return chrono::TYPE(static_cast<chrono::TYPE::rep>(x));            \
  }

    MAKE_LITERAL(nanoseconds, ns)
    MAKE_LITERAL(microseconds, us)
    MAKE_LITERAL(milliseconds, ms)
    MAKE_LITERAL(seconds, s)
    MAKE_LITERAL(minutes, min)
    MAKE_LITERAL(hours, h)
    MAKE_LITERAL(days, days)

#undef MAKE_LITERAL
  }

  namespace chrono {
    template <class Clock, class Duration = typename Clock::duration>
    class time_point {
    public:
      using clock = Clock;
      using duration = Duration;
      using rep = typename Duration::rep;
      using period = typename Clock::period;

      constexpr time_point() = default;  // Default time of clock init

      constexpr explicit time_point(const duration& d) : m_d(d) {}
      constexpr duration time_since_epoch() const { return m_d; }

      time_point& operator=(const time_point&) = default;

      bool operator==(const time_point& rhs) const{
	return m_d == rhs.m_d;
      };

      bool operator!=(const time_point& rhs) const{
	return !(*this == rhs);
      };

      template<typename D>
      time_point& operator = (const time_point<Clock,D>& that){
	m_d = that.time_since_epoch();
	return (*this);
      }
      
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

    template <class C, class D, class Rep, class Period>
    constexpr auto operator+(const time_point<C, D>& lhs, const duration<Rep, Period>& rhs) {
      auto ans = lhs.time_since_epoch() + rhs;
      return time_point<C, decltype(ans)>(ans);
    }

    template <class C, class D, class Rep, class Period>
    constexpr auto operator-(const time_point<C, D>& lhs, const duration<Rep, Period>& rhs) {
      auto ans = lhs.time_since_epoch() - rhs;
      return time_point<C, decltype(ans)>(ans);
    }

    template <class Rep1, class Period1>
    constexpr auto operator-(const duration<Rep1, Period1>& arg) {
      return duration<Rep1, Period1>(-arg.count());
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    constexpr auto operator-(const duration<Rep1, Period1>& lhs,
                             const duration<Rep2, Period2>& rhs) {
      return lhs + (-rhs);
    }

    template <class Rep1, class Period1, class Rep2, class Period2>
    constexpr auto operator+(const duration<Rep1, Period1>& lhs,
                             const duration<Rep2, Period2>& rhs) {
      // So we have:
      // x * a/b + y * c/d = (xda + ybc)/(bd)
      // let:
      // g = gcd(da, bc)
      // then:
      // x * a/b + y * c/d = (xda/g + ybc/g) * g/(bd);
      // I.e. the resulting period is g/bd.
      constexpr auto g = gcd(Period2::den * Period1::num, Period1::den * Period2::num);
      using new_period = ratio_t<g, Period1::den * Period2::den>;

      return duration<int64_t, new_period>(duration<int64_t, new_period>(lhs).count() +
                                           duration<int64_t, new_period>(rhs).count());
    }
  }  // namespace chrono
}  // namespace xtd

#endif
