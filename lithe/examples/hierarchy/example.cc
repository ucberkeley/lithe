#if PTHREAD_BARRIER==1
#include <pthread.h>
#define lithe_barrier_t pthread_barrier_t
#define lithe_barrier_init(barrier, nthreads) pthread_barrier_init(barrier, NULL, nthreads)
#define lithe_barrier_destroy pthread_barrier_destroy
#define lithe_barrier_wait pthread_barrier_wait
#elif SPIN_BARRIER==1
#include "barrier.h"
#define lithe_barrier_t barrier_t
#define lithe_barrier_init barrier_init
#define lithe_barrier_destroy barrier_destroy
#define lithe_barrier_wait barrier_wait
#else
#include "lithe_barrier.h"
#endif
#include "spmd.h"
#include "atomic.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <iostream>


using namespace std;


typedef struct {
  int nthreads;
  lithe_barrier_t barrier;
  int sum;
  int level;
} data_t;


int expected(int nchildren, int nthreads) 
{
  int mult = 0;
  for (int i = 0; i <= nchildren; i++) {
    mult += pow(nthreads, i);
  }
  return (mult * (((nthreads - 1) * nthreads) / 2));      
}


void lockstep(void *__parent) 
{
  int sum = 0;

  data_t *parent = (data_t *)__parent;
  assert(parent != NULL);

  /* create and run children */
  if (parent->level > 0) {
    /* initialize */
    data_t children;  
    children.nthreads = parent->nthreads;
    lithe_barrier_init(&children.barrier, children.nthreads);
    children.sum = 0;
    children.level = parent->level - 1;
    /* spawn children threads */
#if 0
    spmd_spawn(children.nthreads, lockstep, (void *)&children);
#else
    spmd_init(children.nthreads);
    spmd_spawn(lockstep, (void *)&children);
    spmd_fini();
#endif
    /* make sure that children completed their work */
    sum = children.sum;
    assert(sum == expected(children.level, children.nthreads));
    /* clean up */
    lithe_barrier_destroy(&children.barrier);
  }

  /* do own work */
  atomic_add(&parent->sum, spmd_tid());
  lithe_barrier_wait(&parent->barrier);
  assert(parent->sum == (((parent->nthreads - 1) * parent->nthreads) / 2));
  lithe_barrier_wait(&parent->barrier);
  atomic_add(&parent->sum, sum);
  lithe_barrier_wait(&parent->barrier);
  assert(parent->sum == ((((parent->nthreads - 1) * parent->nthreads) / 2) + 
			 (parent->nthreads * sum)));
}


int main(int argc, char **argv) 
{
  data_t data;
  
#if 1
  for (int nchildren = 0; nchildren < 3; nchildren++) {
    for (int nthreads = 1; nthreads <= get_nprocs() * 2; nthreads++) {
#else
  {
    {
      int nchildren = ((argc > 1) ? atoi(argv[1]) : 2); 
      int nthreads = ((argc > 2) ? atoi(argv[2]) : get_nprocs() * 2);
#endif
      cout << "NCHILDREN = " << nchildren << ", NTHREADS = " << nthreads << endl;
      data.nthreads = nthreads;
      lithe_barrier_init(&data.barrier, data.nthreads);
      data.sum = 0;
      data.level = nchildren;
#if 0
      spmd_spawn(data.nthreads, lockstep, (void *)&data);
#else
      spmd_init(data.nthreads);
      spmd_spawn(lockstep, (void *)&data);
      spmd_fini();
#endif
      assert(data.sum == expected(nchildren, nthreads));
      lithe_barrier_destroy(&data.barrier);
    }
  }

  cout << "SUCCESS" << endl;
  return 0;
}
