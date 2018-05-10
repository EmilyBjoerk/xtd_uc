#ifndef XTD_UC_SCHED_FCFS_HPP
#define XTD_UC_SCHED_FCFS_HPP
#include "common.hpp"

#include "cstdint.hpp"
#include "queue.hpp"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/atomic.h>

namespace xtd {
  // A non-preemptive, First Come First Serve (FCFS) scheduler.
  //
  // At each scheduling event (previous task completed, resume from
  // wait, system start) the oldest scheduled task is executed until
  // it completes, without pre-emption. When a task starts execution,
  // global interrupts are always enabled.
  //
  // Note that it may be necessary to annotate your task functions with
  // __attribute__((used)) to prevent the linker removing it if there
  // are no direct calls to it outside of the scheduler.
  //
  // If the system is overloaded, new tasks will not be admitted.
  //
  // It does not support scheduling tasks in the future. For this reason
  // this scheduler is mainly useful with an interrupt driven design
  // where an interrupt schedules a new task. If the scheduler was sleeping
  // upon return from the ISR the MCU will be awake again and scheduling
  // continues.
  //
  // A fast polling with sporadic other tasks can be realised by having your
  // polling task schedule itself after it is done and let IRQs schedule
  // your other sporadic tasks.
  //
  // This code assumes single threaded MCU.
  template <fast_size_t max_tasks_>
  class fcfs_scheduler {
  public:
    using task_type = void (*)(void);

    __attribute__((noreturn)) void run() {
      // Interrupts are disabled while we're in the main body and enabled
      // when we go to sleep or execute a task (i.e. most of the time)
      cli();

      while (true) {
        if (tasks.empty()) {
          sleep_enable();
          set_sleep_mode(SLEEP_MODE_IDLE);
          sei();  // Interrups enabled while sleeping
          sleep_cpu();
          cli();
          sleep_disable();
        } else {
          auto t = tasks.get();
          sei();  // Interrupts enabled while executing task
          t();
          cli();
        }
      }
    }

    // Schedules the task to be executed after priori scheduled tasks are done.
    // Returns false if the task was not admitted due to overload.
    // May be called safely from ISRs.
    bool schedule(task_type t) {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (!tasks.full()) {
          tasks.push(t);
          return true;
        }
        return false;
      }
    }

  private:
    xtd::queue<task_type, max_tasks_> tasks;
  };

}  // namespace xtd

#endif
