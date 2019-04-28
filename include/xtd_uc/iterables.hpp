#ifndef XTD_UC_ITERABLES_HPP
#define XTD_UC_ITERABLES_HPP
#include "common.hpp"

#include <iterator>

namespace xtd {
  namespace detail {
    template <typename T>
    struct reversion_wrapper {
      T& iterable;
    };

    template <typename T>
    auto begin(reversion_wrapper<T> w) {
      return std::rbegin(w.iterable);
    }

    template <typename T>
    auto end(reversion_wrapper<T> w) {
      return std::rend(w.iterable);
    }
  }  // namespace detail

  template <typename T>
  detail::reversion_wrapper<T> reverse(T&& iterable) {
    return {iterable};
  }
}  // namespace xtd

#endif
