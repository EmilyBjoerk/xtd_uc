#ifndef XTD_UC_TUPLE_HPP
#define XTD_UC_TUPLE_HPP
#include "common.hpp"

#include "cstdint.hpp"
#include "utility.hpp"

namespace xtd {
  namespace detail {

    template <typename...>
    struct pack;

    template <fast_size_t i, typename first, typename... types>
    struct pack_get {
      static const auto& get(const pack<first, types...>& pack) {
        return pack_get<i - 1, types...>::get(pack.next);
      }

      static auto& get(pack<first, types...>& pack) {
        return pack_get<i - 1, types...>::get(pack.next);
      }
    };

    template <typename first, typename... types>
    struct pack_get<0, first, types...> {
      static const auto& get(const pack<first, types...>& pack) { return pack.value; }
      static auto& get(pack<first, types...>& pack) { return pack.value; }
    };

    template <typename type>
    struct pack<type> {
      constexpr static fast_size_t size() { return 1; }

      constexpr pack() = default;
      template <typename arg>
      constexpr explicit pack(arg&& a) : value(forward<arg>(a)) {}

    private:
      type value;

      template <fast_size_t, typename, typename...>
      friend struct pack_get;
    };

    template <typename type, typename... types>
    struct pack<type, types...> {
      using next_pack = pack<types...>;

      constexpr pack() = default;
      template <typename arg, typename... args>
      constexpr pack(arg&& a, args&&... as) : value(forward<arg>(a)), next(forward<args>(as)...) {}

      constexpr static fast_size_t size() { return 1 + next_pack::size(); }

    private:
      type value;
      next_pack next;

      template <fast_size_t, typename, typename...>
      friend struct pack_get;
    };
  }  // namespace detail

  template <typename... types>
  using tuple = detail::pack<types...>;

  template <fast_size_t i, typename... types>
  constexpr const auto& get(const tuple<types...>& tuple) {
    return detail::pack_get<i, types...>().get(tuple);
  }

  template <fast_size_t i, typename... types>
  constexpr auto& get(tuple<types...>& tuple) {
    return detail::pack_get<i, types...>().get(tuple);
  }

  template <typename... types>
  constexpr auto make_tuple(types&&... args) {
    return tuple<decay_t<types>...>(forward<types>(args)...);
  }

}  // namespace xtd

#endif
