#if PTHREAD_BARRIER==1
#include <pthread.h>
#define lithe_barrier_t pthread_barrier_t
#define lithe_barrier_init(barrier, nthreads) pthread_barrier_init(barrier, NULL, nthreads)
#define lithe_barrier_destroy pthread_barrier_destroy
#define lithe_barrier_wait pthread_barrier_wait
#elif SPIN_BARRIER==1
#include "spin_barrier.h"
#define lithe_barrier_t spin_barrier_t
#define lithe_barrier_init spin_barrier_init
#define lithe_barrier_destroy spin_barrier_destroy
#define lithe_barrier_wait spin_barrier_wait
#else
#include "lithe_barrier.h"
#endif
#include "spmd.h"
#include "atomic.h"
#include <assert.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <iostream>


using namespace std;


typedef struct {
  int nthreads;
  lithe_barrier_t barrier;
  int sum;
} data_t;


void lockstep(void *__dptr) 
{
  data_t *dptr = (data_t *)__dptr;
  assert(dptr != NULL);
  atomic_add(&dptr->sum, spmd_tid());
  lithe_barrier_wait(&dptr->barrier);
  assert(dptr->sum == (((dptr->nthreads - 1) * dptr->nthreads) / 2));
  lithe_barrier_wait(&dptr->barrier);
  atomic_add(&dptr->sum, spmd_tid());
  lithe_barrier_wait(&dptr->barrier);
  assert(dptr->sum == ((dptr->nthreads - 1) * dptr->nthreads));  
}


int main(int argc, char **argv) 
{
  data_t data;
#if 0
  for (data.nthreads = 1; data.nthreads <= get_nprocs() * 3; data.nthreads += 1) {
#else
  {
    data.nthreads = ((argc > 1) ? atoi(argv[1]) : get_nprocs());
#endif
    lithe_barrier_init(&data.barrier, data.nthreads);
    data.sum = 0;
    spmd_spawn(data.nthreads, lockstep, (void *)&data);
    assert(data.sum == ((data.nthreads - 1) * data.nthreads));  
    lithe_barrier_destroy(&data.barrier);
  }
  cout << "SUCCESS" << endl;
  return 0;
}
