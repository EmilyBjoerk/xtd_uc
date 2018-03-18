#include "xtd_uc/common.hpp"

#include "xtd_uc/cstdint.hpp"
#include "xtd_uc/queue.hpp"

#include <avr/sleep.h>
#include <util/atomic.h>

namespace xtd {
  /*
    A non-preemptive, First Come First Serve (FCFS) scheduler.

    At each scheduling event (previous task completed, resume from
    wait, system start) the oldes scheduled task is executed until
    it completes, without pre-emption.

    If the system is overloaded, new tasks will not be admitted.

    It does not support scheduling tasks in the future.

    This code assumes single thread MCU with IRQs enabled.
  */
  template <fast_size_t max_tasks_>
  class fcfs_scheduler {
  public:
    using task = void (*)(void);

    void run() __attribute__((noreturn)) {
      while (true) {
        bool notasks;
        ATOMIC_BLOCK(ATOMIC_FORCEON) { notasks = tasks.empty(); }

        if (notasks) {
          ATOMIC_BLOCK(ATOMIC_FORCEON) {
            sleep_enable();
            set_sleep_mode(SLEEP_MODE_IDLE);
          }
          sleep_cpu();
          sleep_disable();
        } else {
          task t;
          ATOMIC_BLOCK(ATOMIC_FORCEON) { t = tasks.get(); }
          t();
        }
      }
    }

    bool schedule(task t) {
      if (!tasks.full()) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { tasks.push(t); }
        return true;
      }
      return false;
    }

  private:
    xtd::queue<task, max_tasks_> tasks;
  };

}  // namespace xtd
