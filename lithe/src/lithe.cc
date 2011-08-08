#include "lithe.hh"

namespace lithe {


void __enter(void *__this)
{
  (reinterpret_cast<Scheduler *>(__this))->enter();
}


void __yield(void *__this, lithe_sched_t *child)
{
  (reinterpret_cast<Scheduler *>(__this))->yield(child);
}


void __reg(void *__this, lithe_sched_t *child)
{
  (reinterpret_cast<Scheduler *>(__this))->reg(child);
}


void __unreg(void *__this, lithe_sched_t *child)
{
  (reinterpret_cast<Scheduler *>(__this))->unreg(child);
}


void __request(void *__this, lithe_sched_t *child, int k)
{
  (reinterpret_cast<Scheduler *>(__this))->request(child, k);
}


void __unblock(void *__this, lithe_task_t *task)
{
  (reinterpret_cast<Scheduler *>(__this))->unblock(task);
}


const lithe_sched_funcs_t Scheduler::funcs = {
  /* .enter   = */__enter,
  /* .yield   = */__yield,
  /* .reg     = */__reg,
  /* .unreg   = */__unreg,
  /* .request = */__request,
  /* .unblock = */__unblock,
};


Scheduler::~Scheduler() {}
 
}

