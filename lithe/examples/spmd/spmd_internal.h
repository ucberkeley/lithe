#ifndef SPMD_INTERNAL_H
#define SPMD_INTERNAL_H


#include "spmd.h"
#include "child2.h"
#include "queue.h"
#include "lithe.hh"
#include "spinlock.h"
#include <vector>
#include <stdint.h>
#include <assert.h>


using namespace std;


typedef vector<void *> spmd_tls_t;


class SPMD_Scheduler : public lithe::Scheduler 
{
 public:
  SPMD_Scheduler(int __nthreads, void (*__func)(void *) = NULL, void *__arg = NULL);
  ~SPMD_Scheduler();
 protected:
  void spawn();
  void enter_begin();
 protected:
  void enter();
  void yield(lithe_sched_t *child);
  void reg(lithe_sched_t *child);
  void unreg(lithe_sched_t *child);
  void request(lithe_sched_t *child, int k);
  void unblock(lithe_task_t *task);
 protected:
  int nthreads;         /* total # of threads to spawn */
  void (*func)(void *); /* function to be executed on all threads */
  void *arg;            /* argument to function */
  int tls_sz;           /* size of tls */   
  lithe_task_t *task0;  /* initial task for 2-phased init/reg */
 private:
  int spawned;          /* # of spawned threads */
  int completed;        /* # of completed threads */
  int unreg_once;       /* once var to determine which thread unregisters */
  Queue<lithe_task_t *> *unblocked; /* unblocked tasks to resume */
  children_t children;  /* list of children */
  int nchildren;        /* # of children registered */
 private:
  friend void spmd_spawn(int nthreads, void (*func)(void *), void *arg);
  friend void spmd_spawn(void (*func)(void *), void *arg);
  friend void spmd_init(int nthreads);
  friend void spmd_fini();
  friend int spmd_tid();
  friend spmd_key_t spmd_tls_alloc();
  friend void * spmd_tls_get(spmd_key_t key);
  friend void spmd_tls_set(spmd_key_t key, void *val);
  friend void __spawn(void *__sched);
  friend void __spawn(lithe_task_t *task, void *__sched);
  friend void __enter_begin(void *__sched);
};


void __spawn(void *__sched);
void __spawn(lithe_task_t *task, void *__sched);
void __enter_begin(void *__sched);
void __enter_end(lithe_task_t *task, void *__ignore);


#endif  // SPMD_INTERNAL_H
