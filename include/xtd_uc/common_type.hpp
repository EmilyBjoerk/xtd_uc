#ifndef XTD_UC_COMMON_TYPE_HPP
#define XTD_UC_COMMON_TYPE_HPP
#include "common.hpp"

namespace xtd {
  // primary template (used for zero types)
  template <class... T>
  struct common_type {};

  //////// one type
  template <class T>
  struct common_type<T> {
    using type = std::decay_t<T>;
  };

  //////// two types

  // default implementation for two types
  template <class T1, class T2>
  using cond_t = decltype(false ? std::declval<T1>() : std::declval<T2>());

  template <class T1, class T2, class = void>
  struct common_type_2_default {};

  template <class T1, class T2>
  struct common_type_2_default<T1, T2, std::void_t<cond_t<T1, T2>>> {
    using type = std::decay_t<cond_t<T1, T2>>;
  };

  // dispatcher to decay the type before applying specializations
  template <class T1, class T2, class D1 = std::decay_t<T1>, class D2 = std::decay_t<T2>>
  struct common_type_2_impl : common_type<D1, D2> {};

  template <class D1, class D2>
  struct common_type_2_impl<D1, D2, D1, D2> : common_type_2_default<D1, D2> {};

  template <class T1, class T2>
  struct common_type<T1, T2> : common_type_2_impl<T1, T2> {};

  //////// 3+ types

  template <class AlwaysVoid, class T1, class T2, class... R>
  struct common_type_multi_impl {};

  template <class T1, class T2, class... R>
  struct common_type_multi_impl<std::void_t<common_type_t<T1, T2>>, T1, T2, R...>
      : common_type<common_type_t<T1, T2>, R...> {};

  template <class T1, class T2, class... R>
  struct common_type<T1, T2, R...> : common_type_multi_impl<void, T1, T2, R...> {};
}  // namespace xtd
#endif
