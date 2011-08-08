#include "spin_barrier.h"
#include "atomic.h"
#include <stdlib.h>
#include <assert.h>


void spin_barrier_init(spin_barrier_t *barrier, int nthreads) 
{
  assert(barrier != NULL);
  barrier->N = nthreads;
  barrier->arrived = 0;
  barrier->signals = (padded_bool_t *)calloc(nthreads, sizeof(padded_bool_t));
  barrier->wait = false;
}

void spin_barrier_destroy(spin_barrier_t *barrier) 
{
  assert(barrier != NULL);
  free(barrier->signals);
}

void spin_barrier_wait(spin_barrier_t *barrier) 
{
  assert(barrier != NULL);
  bool wait = barrier->wait;
  int id = fetchadd(&barrier->arrived, 1);
  if (id == (barrier->N - 1)) {
    barrier->arrived = 0;
    barrier->wait = !barrier->wait;
    for (int i = 0; i < barrier->N; i++) {
      barrier->signals[i].val = !wait;
    }
  } else {
    while (coherentread(&barrier->signals[id].val) == wait) { }
  }
}


