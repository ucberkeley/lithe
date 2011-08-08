#ifndef LITHE_HH
#define LITHE_HH

#include "lithe.h"

namespace lithe {

class Scheduler {
 private:
  friend void __enter(void *__this);
  friend void __yield(void *__this, lithe_sched_t *child);
  friend void __reg(void *__this, lithe_sched_t *child);
  friend void __unreg(void *__this, struct lithe_sched *child);
  friend void __request(void *__this, lithe_sched_t *child, int k);
  friend void __unblock(void *__this, lithe_task_t *task);
  
 protected:
  virtual void enter() = 0;
  virtual void yield(lithe_sched_t *child) = 0;
  virtual void reg(lithe_sched_t *child) = 0;
  virtual void unreg(lithe_sched_t *child) = 0;
  virtual void request(lithe_sched_t *child, int k) = 0;
  virtual void unblock(lithe_task_t *task) = 0;
  
  static const lithe_sched_funcs_t funcs;

 public:
//   int doEnter(lithe_sched_t *child) { return lithe_sched_enter(child); }
//   int doYield() { return lithe_sched_yield(); }
//   int doRegister() { return lithe_sched_register(&funcs, (void *) this); }
//   int doUnregister() { return lithe_sched_unregister(); }
//   int doRequest(int k) { return lithe_sched_request(k); }
//   /* TODO(benh): What sorts of sweet C++ tricks can we play for block? */
//   int doTaskBlock() { return -1; }
//   int doTaskUnblock(lithe_task_t *task) { return
//   lithe_task_unblock(task); }

  virtual ~Scheduler() = 0;
};

}

#endif  // LITHE_HH
