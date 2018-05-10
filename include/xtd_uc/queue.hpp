#ifndef XTD_UC_QUEUE_HPP
#define XTD_UC_QUEUE_HPP
#include "common.hpp"

#include "cstdint.hpp"

namespace xtd {

  // A bounded queue implemented using a circular buffer.
  //
  // The type of T must be:
  // * default constructible
  // * trivially destructible
  // * copy assignible
  //
  // Upon instantiation, a buffer of N+1 elements is default constructed.
  //
  // For best performance, make sure N = 2^k - 1 for some k. This makes sure
  // that the code is free from divisions and multiplications.
  template <typename T, fast_size_t N>
  class queue {
  public:
    using size_type = fast_size_t;

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

    // Empties the queue (objects in the queue are not destructed)
    void clear() {
      read = 0;
      write = 0;
    }

    // Pushes a new value onto the queue.
    // If the queue is full() then pushing a new value is undefined behaviour.
    void push(const T& v) {
      buffer[write] = v;
      write = inc(write);
    }

    // Removes the front element from the queue.
    void pop() { read = inc(read); }

    // Returns true if the queue is full and unable to accept another element.
    bool full() const { return size() == capacity(); }

    // Returns true if the queue is empty. Prefer this to using size() == 0 as this is slightly
    // faster.
    bool empty() const { return read == write; }

    // Steal the next element from the queue (the element is removed from the queue).
    // If the queue is empty() then this causes undefined behaviour.
    T get() {
      auto ans = peek();
      pop();
      return ans;
    }

    // Look at the next element of the queue without removing it.
    // IF the queue is empty() then this causes undefined behaviour.
    T& peek() { return buffer[read]; }

  private:
    T buffer[N + 1];
    size_type read = 0;
    size_type write = 0;

    size_type inc(size_type i) { return size_type((i + 1) % (N + 1)); }
  };
}  // namespace xtd

#endif
