#ifndef XTD_UC_TMPL_HPP
#define XTD_UC_TMPL_HPP

namespace xtd {
  namespace tmpl {
    // -------------------------------------------------------------------------
    // Builds a TMP counter.
    //
    // Example:
    // counter::index == 0
    // increment<increment<increment<counter>>>::index == 3
    // -------------------------------------------------------------------------
    template <typename N = void, int offset = 0>
    struct increment {
      using parent = N;
      constexpr static int index = 1 + N::index;
    };

    template <int offset>
    struct increment<void, offset> {
      constexpr static int index = offset;
    };

    template <int offset = 0>
    using counter = increment<void, offset>;

    // -------------------------------------------------------------------------
    // Builds a linked list of const strings.
    //
    // Example:
    // constexpr char str1[] = "hello";
    // constexpr char str2[] = "template";
    // constexpr char str3[] = "list";
    // using hello = string_list<str1>; // index 0
    // using template = string_list_add<hello, str2>; // index 1
    // using list = string_list_add<template, str3>; // index 2
    //
    // string_list_get<1, list>::string == str2;
    // -------------------------------------------------------------------------
    template <typename N, const char* S>
    struct string_list_add {
      using next = N;
      constexpr static int index = 1 + N::index;
      constexpr static const char* string = S;
    };

    template <const char* S>
    struct string_list_add<void, S> {
      constexpr static int index = 0;
      constexpr static const char* string = S;
    };

    template <const char* S>
    using string_list = string_list_add<void, S>;

    // -------------------------------------------------------------------------
    // This struct essentially allows random acces into a list of types
    // constructed by each having a type called 'next' which points to the next
    // type in the list.
    // -------------------------------------------------------------------------
    namespace detail {
      template <int i, typename T>
      struct get_type_impl {
        using type = typename get_type_impl<i - 1, typename T::next>::type;
      };

      template <typename T>
      struct get_type_impl<0, T> {
        using type = T;
      };
    }  // namespace detail
    template <int i, typename T>
    using get_type = typename detail::get_type_impl<T::index - i, T>::type;

  }  // namespace tmpl
}  // namespace xtd

#endif
