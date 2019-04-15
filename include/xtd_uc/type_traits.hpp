#ifndef XTD_UC_TYPE_TRAITS_HPP
#define XTD_UC_TYPE_TRAITS_HPP
#include "common.hpp"

#include "cstdint.hpp"

namespace xtd {

  // ---------------------------------------------------------------------------
  // enable_if
  // ---------------------------------------------------------------------------
  template <bool B, class T = void>
  struct enable_if {};

  template <class T>
  struct enable_if<true, T> {
    typedef T type;
  };

  template <bool B, class T = void>
  using enable_if_t = typename enable_if<B, T>::type;

  // ---------------------------------------------------------------------------
  // conditional
  // ---------------------------------------------------------------------------
  template <bool B, class T, class F>
  struct conditional {
    typedef T type;
  };

  template <class T, class F>
  struct conditional<false, T, F> {
    typedef F type;
  };

  template <bool B, class T, class F>
  using conditional_t = typename conditional<B, T, F>::type;

  // ---------------------------------------------------------------------------
  // integral & bool constants
  // ---------------------------------------------------------------------------
  template <class T, T v>
  struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant;
    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
  };

  template <bool B>
  using bool_constant = integral_constant<bool, B>;

  using true_type = bool_constant<true>;
  using false_type = bool_constant<false>;

  // ---------------------------------------------------------------------------
  // is_function
  // ---------------------------------------------------------------------------
  // primary template
  template <class>
  struct is_function : false_type {};

  // specialization for regular functions
  template <class Ret, class... Args>
  struct is_function<Ret(Args...)> : true_type {};

  // specialization for variadic functions such as printf
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...)> : true_type {};

  // specialization for function types that have cv-qualifiers
  template <class Ret, class... Args>
  struct is_function<Ret(Args...) const> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args...) volatile> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args...) const volatile> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...) const> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...) volatile> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...) const volatile> : true_type {};

  // specialization for function types that have ref-qualifiers
  template <class Ret, class... Args>
  struct is_function<Ret(Args...)&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args...) const&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args...) volatile&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args...) const volatile&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...)&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...) const&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...) volatile&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...) const volatile&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args...) &&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args...) const&&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args...) volatile&&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args...) const volatile&&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...) &&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...) const&&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...) volatile&&> : true_type {};
  template <class Ret, class... Args>
  struct is_function<Ret(Args..., ...) const volatile&&> : true_type {};

  // specializations for noexcept versions of all the above (C++17 and later)
  /*
  template <class Ret, class... Args>
struct is_function<Ret(Args...) noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) volatile noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const volatile noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) & noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const & noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile& noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile& noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) & noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const & noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) volatile& noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const volatile& noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) && noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const && noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) volatile&& noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args...) const volatile&& noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) && noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const && noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) volatile&& noexcept> : true_type {};
template <class Ret, class... Args>
struct is_function<Ret(Args......) const volatile&& noexcept> : true_type {};
  */

  template <class T>
  /*inline*/ constexpr bool is_function_v = is_function<T>::value;

  // ---------------------------------------------------------------------------
  // is_same
  // ---------------------------------------------------------------------------
  template <class T, class U>
  struct is_same : false_type {};

  template <class T>
  struct is_same<T, T> : true_type {};

  template <class T, class U>
  /*inline*/ constexpr bool is_same_v = is_same<T, U>::value;

  // ---------------------------------------------------------------------------
  // reference modifications
  // ---------------------------------------------------------------------------
  template <class T>
  struct add_lvalue_reference {
    using type = T&;
  };

  template <class T>
  struct add_rvalue_reference {
    using type = T&&;
  };

  template <class T>
  using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

  template <class T>
  using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

  static_assert(is_same_v<int&, add_lvalue_reference_t<int>>, "");
  static_assert(is_same_v<int&, add_lvalue_reference_t<int&>>, "");
  static_assert(is_same_v<int&, add_lvalue_reference_t<int&&>>, "");

  static_assert(is_same_v<int&&, add_rvalue_reference_t<int>>, "");
  static_assert(is_same_v<int&, add_rvalue_reference_t<int&>>, "");
  static_assert(is_same_v<int&&, add_rvalue_reference_t<int&&>>, "");

  template <class T>
  struct remove_reference {
    typedef T type;
  };

  template <class T>
  struct remove_reference<T&> {
    typedef T type;
  };

  template <class T>
  struct remove_reference<T&&> {
    typedef T type;
  };

  template <class T>
  using remove_reference_t = typename remove_reference<T>::type;

  template <class T>
  struct is_lvalue_reference : false_type {};
  template <class T>
  struct is_lvalue_reference<T&> : true_type {};

  template <class T>
  /*inline*/ constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

  template <class T>
  struct is_rvalue_reference : false_type {};
  template <class T>
  struct is_rvalue_reference<T&&> : true_type {};
  template <class T>
  /*inline*/ constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

  static_assert(is_lvalue_reference_v<add_lvalue_reference_t<int>>, "");
  static_assert(is_rvalue_reference_v<add_rvalue_reference_t<int>>, "");
  static_assert(!is_lvalue_reference_v<add_rvalue_reference_t<int>>, "");
  static_assert(!is_rvalue_reference_v<add_lvalue_reference_t<int>>, "");

  // ---------------------------------------------------------------------------
  // pointer modifications
  // ---------------------------------------------------------------------------
  namespace detail {
    template <class T, bool is_function_type = false>
    struct add_pointer {
      using type = typename remove_reference<T>::type*;
    };

    template <class T>
    struct add_pointer<T, true> {
      using type = T;
    };

    template <class T, class... Args>
    struct add_pointer<T(Args...), true> {
      using type = T (*)(Args...);
    };

    template <class T, class... Args>
    struct add_pointer<T(Args..., ...), true> {
      using type = T (*)(Args..., ...);
    };

  }  // namespace detail

  template <class T>
  struct add_pointer : detail::add_pointer<T, is_function<T>::value> {};

  template <class T>
  using add_pointer_t = typename add_pointer<T>::type;

  // ---------------------------------------------------------------------------
  // cv modifications
  // ---------------------------------------------------------------------------
  template <class T>
  struct remove_const {
    typedef T type;
  };

  template <class T>
  struct remove_const<const T> {
    typedef T type;
  };

  template <class T>
  struct remove_volatile {
    typedef T type;
  };

  template <class T>
  struct remove_volatile<volatile T> {
    typedef T type;
  };

  template <class T>
  struct remove_cv {
    typedef typename remove_volatile<typename remove_const<T>::type>::type type;
  };

  // TODO: Implement is_convertible. Then uncomment section in xtd::forward.
  //  template <class F, class T>
  ///*inline*/ constexpr bool is_convertible_v = is_convertible<F, T>::value;

  // ---------------------------------------------------------------------------
  // is_array
  // ---------------------------------------------------------------------------
  template <class T>
  struct is_array : false_type {};

  template <class T>
  struct is_array<T[]> : true_type {};

  template <class T, size_t N>
  struct is_array<T[N]> : true_type {};

  template <class T>
  /*inline*/ constexpr bool is_array_v = is_array<T>::value;

  template <class T>
  struct remove_extent {
    typedef T type;
  };

  template <class T>
  struct remove_extent<T[]> {
    typedef T type;
  };

  template <class T, size_t N>
  struct remove_extent<T[N]> {
    typedef T type;
  };

  template <class T>
  using remove_extent_t = typename remove_extent<T>::type;

  // ---------------------------------------------------------------------------
  // decay
  // ---------------------------------------------------------------------------
  template <class T>
  struct decay {
  private:
    typedef typename remove_reference<T>::type U;

  public:
    typedef
        typename conditional<is_array_v<U>, typename remove_extent<U>::type*,
                             typename conditional<is_function_v<U>, typename add_pointer<U>::type,
                                                  typename remove_cv<U>::type>::type>::type type;
  };

  template <class T>
  using decay_t = typename decay<T>::type;

  // ---------------------------------------------------------------------------
  // numerical tests
  // ---------------------------------------------------------------------------

  template <class T>
  struct is_floating_point
      : xtd::integral_constant<
            bool, xtd::is_same<float, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<double, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<long double, typename xtd::remove_cv<T>::type>::value> {};

  template <class T>
  struct is_integral
      : xtd::integral_constant<
            bool, xtd::is_same<bool, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<char, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<char16_t, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<char32_t, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<wchar_t, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<short, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<int, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<long, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<long long, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<unsigned char, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<unsigned short, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<unsigned int, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<unsigned long, typename xtd::remove_cv<T>::type>::value ||
                      xtd::is_same<unsigned long long, typename xtd::remove_cv<T>::type>::value> {};

  template <class T>
  struct is_arithmetic : xtd::integral_constant<bool, xtd::is_integral<T>::value ||
                                                          xtd::is_floating_point<T>::value> {};

  namespace detail {
    template <typename T, bool = xtd::is_arithmetic<T>::value>
    struct is_signed : xtd::integral_constant<bool, T(-1) < T(0)> {};

    template <typename T>
    struct is_signed<T, false> : xtd::false_type {};
  }  // namespace detail

  template <typename T>
  struct is_signed : detail::is_signed<T>::type {};

}  // namespace xtd

#endif
