#ifndef XTD_UC_QUEUE_HPP
#define XTD_UC_QUEUE_HPP
#include "common.hpp"

#include <stdint.h>

namespace xtd {

  // If N = 2^k -1, then the operation of the queue will be without divisions.
  template <typename T, uint8_t N>
  class queue {
  public:
    using size_type = uint8_t;

    size_type capacity() const { return N; }

    size_type size() const {
      if (empty()) {
        return size_type();
      } else if (write > read) {
        return size_type(write - read);  // Cannot overflow
      } else {
        return size_type(N - read + write + 1);  // Cannot overflow (N > read > write)
      }
    }

    void clear() {
      read = 0;
      write = 0;
    }

    void push(const T& v) {
      buffer[write] = v;
      write = inc(write);
    }
    void pop() { read = inc(read); }
    bool full() const { return size() == capacity(); }
    bool empty() const { return read == write; }
    T get() {
      auto ans = peek();
      pop();
      return ans;
    }
    T& peek() { return buffer[read]; }

  private:
    T buffer[N + 1];
    size_type read = 0;
    size_type write = 0;

    size_type inc(size_type i) { return size_type((i + 1) % (N + 1)); }
  };
}
#endif
