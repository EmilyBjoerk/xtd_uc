#include "xtd_uc/tmpl.hpp"
#include <gtest/gtest.h>

using namespace xtd;

using count0 = tmpl::counter<>;
using count1 = tmpl::increment<count0>;
using count10 = tmpl::counter<10>;
using count11 = tmpl::increment<count10>;

static_assert(count0::index == 0, "");
static_assert(count1::index == 1, "");
static_assert(count10::index == 10, "");
static_assert(count11::index == 11, "");

constexpr char str0[] = "hello";
constexpr char str1[] = "template";
constexpr char str2[] = "list";

TEST(StringList, GetStrings) {
  using string0 = tmpl::string_list<str0>;               // index 0
  using string1 = tmpl::string_list_add<string0, str1>;  // index 1
  using string2 = tmpl::string_list_add<string1, str2>;  // index 2

  static_assert(0 == string0::index, "");
  static_assert(1 == string1::index, "");
  static_assert(2 == string2::index, "");

  ASSERT_STREQ(str0, (tmpl::get_type<0, string2>::string));
  ASSERT_STREQ(str1, (tmpl::get_type<1, string2>::string));
  ASSERT_STREQ(str2, (tmpl::get_type<2, string2>::string));
}
