#include "spmd_internal.h"
#include "task.h"
#include "atomic.h"
#include <stdlib.h>


#include <iostream>
using namespace std;


void spmd_spawn(int nthreads, void (*func)(void *), void *arg) 
{
  assert(nthreads > 0);
  SPMD_Scheduler *sched = new SPMD_Scheduler(nthreads, func, arg);
  sched->spawn();
  delete sched;
}


void spmd_spawn(void (*func)(void *), void *arg) 
{
  SPMD_Scheduler *sched = (SPMD_Scheduler *)lithe_sched_this();
  assert(sched != NULL);
  sched->func = func;
  sched->arg = arg;
  lithe_task_block(__spawn, (void *)sched);
}


void spmd_init(int nthreads) 
{
  assert(nthreads > 0);
  SPMD_Scheduler *sched = new SPMD_Scheduler(nthreads);
  sched->task0 = task_create();
  lithe_sched_register_task(&SPMD_Scheduler::funcs, (void *)sched, sched->task0);
}


void spmd_fini() 
{
  SPMD_Scheduler *sched = (SPMD_Scheduler *)lithe_sched_this();
  assert(sched != NULL);
  lithe_task_t *task;
  lithe_sched_unregister_task(&task);
  assert(sched->task0 == task);
  task_destroy(task);
  delete sched;
}


int spmd_tid() 
{
  spmd_tls_t *tls;
  lithe_task_gettls((void **)(&tls));
  return ((int)(long)tls->at(0));
}


spmd_key_t spmd_tls_alloc() 
{
  SPMD_Scheduler *sched = (SPMD_Scheduler *)lithe_sched_this();
  assert(sched != NULL);
  spmd_key_t key = fetchadd(&sched->tls_sz, 1);
  return key;
}


void * spmd_tls_get(spmd_key_t key) 
{
  spmd_tls_t *tls;
  lithe_task_gettls((void **)(&tls));
  if (key < tls->size()) {
    return tls->at(key);
  } else {
    assert(0); /* getting unallocated or uninitialized tls entry */
    return NULL;
  }
}


void spmd_tls_set(spmd_key_t key, void *val) 
{
  spmd_tls_t *tls;
  lithe_task_gettls((void **)(&tls));
  if (key < tls->size()) {
    tls->at(key) = val;
  } else {
    SPMD_Scheduler *sched = (SPMD_Scheduler *)lithe_sched_this();
    assert(sched != NULL);
    if (key < sched->tls_sz) {
      tls->resize(sched->tls_sz);
      tls->at(key) = val;
    } else {
      assert(0); /* setting unallocated tls entry */
    }
  }
}


SPMD_Scheduler::SPMD_Scheduler(int __nthreads, void (*__func)(void *), void *__arg) : 
  nthreads(__nthreads),
  func(__func),
  arg(__arg),
  tls_sz(1),
  task0(NULL),
  spawned(0),
  completed(0)
{
  spinlock_init(&unreg_once);
  unblocked = new Queue<lithe_task_t *>(__nthreads);
  children_create(&children, nthreads);
  nchildren = 0;
}


SPMD_Scheduler::~SPMD_Scheduler() 
{
  delete unblocked;
  children_destroy(children);
}


void SPMD_Scheduler::spawn() 
{
  lithe_sched_register(&funcs, (void *)this, __spawn, (void *)this);
}


void SPMD_Scheduler::enter() 
{
  lithe_task_t *task;

  /* nothing left to do: release hart*/
  if (coherentread(&completed) == nthreads) {
    assert(!unblocked->pop(&task));
    assert(nchildren == 0);
    /* designate the first hart to get here as the master to
     * unregister */
    if (spinlock_trylock(&unreg_once) == UNLOCKED) {
      /* registered via lithe_sched_register  */
      if (task0 == NULL) {
	lithe_sched_unregister();
      } 
      /* registered via lithe_sched_register_task */
      else {
	lithe_task_resume(task0);
      }
    } else {
      lithe_sched_yield();
    }
  }

  /* TODO(xoxo): unblocked and children may become bottlenecks */

  /* complete unblocked contexts */
  if (unblocked->pop(&task)) {
    lithe_task_resume(task);
  }

  /* satisfy a child's request */
  lithe_sched_t *sched = children_next(children);
  if (sched != NULL) {
    lithe_sched_enter(sched);
  }

  /* spawn unstarted threads on a new context */
  if (coherentread(&spawned) < nthreads) {
    task = task_create();
    lithe_task_do(task, __enter_begin, (void *)this);
  }

  /* nothing to do (for now): release hart */
  lithe_sched_yield();
}


void SPMD_Scheduler::yield(lithe_sched_t *child) 
{
  lithe_sched_reenter(); 
}


void SPMD_Scheduler::reg(lithe_sched_t *child) 
{
  children_add(children, child);
  atomic_increment(&nchildren);
}


void SPMD_Scheduler::unreg(lithe_sched_t *child) 
{
  children_remove(children, child);
  atomic_decrement(&nchildren);
}


void SPMD_Scheduler::request(lithe_sched_t *child, int k) 
{
  lithe_sched_request(k);
  children_update(children, child, k);
}


void SPMD_Scheduler::unblock(lithe_task_t *task) 
{
  unblocked->push(task);
  lithe_sched_request(1);
}


void SPMD_Scheduler::enter_begin() 
{
  bool done;

  /* spawn next thread */
  int tid = fetchadd(&spawned, 1);
  spmd_tls_t *tls = new spmd_tls_t(1, NULL);
  while (tid < nthreads) {
    /* set thread id */
    tls->at(0) = (void *)tid;
    lithe_task_settls((void *)tls);
    /* execute function */
    func(arg);
    /* bookkeeping: increment the count of completed threads */
    atomic_increment(&completed);
    /* get next thread */
    tid = fetchadd(&spawned, 1);
  }
  delete tls;

  /* no more new threads to spawn */
  /* clean up current context and look for other work */
  lithe_task_block(__enter_end, NULL);
}


void __spawn(void *__sched) 
{
  lithe_sched_request(((SPMD_Scheduler *)__sched)->nthreads);
  ((SPMD_Scheduler *)__sched)->enter();
}


void __spawn(lithe_task_t *task, void *__sched) 
{
  SPMD_Scheduler *sched = (SPMD_Scheduler *)__sched;
  assert(sched != NULL);
  assert(sched->task0 == task);
  lithe_sched_request(sched->nthreads);
  sched->enter();
}


void __enter_begin(void *__sched) 
{
  assert(__sched != NULL);
  ((SPMD_Scheduler *)__sched)->enter_begin();
}


void __enter_end(lithe_task_t *task, void *__ignore) 
{
  task_destroy(task);
  lithe_sched_reenter();
}








  
  



