#include "xtd_uc/queue.hpp"
#include <gtest/gtest.h>

TEST(Queue, Full) {
  constexpr auto n = 31;
  xtd::queue<char, n> cut;

  for (int i = 0; i < n - 1; ++i) {
    cut.push(i);
  }

  ASSERT_FALSE(cut.full());

  for (int i = 0; i < n; ++i) {
    cut.push(i);
    ASSERT_TRUE(cut.full());
    cut.pop();
    ASSERT_FALSE(cut.full());
  }
}
