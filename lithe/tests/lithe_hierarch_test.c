/*
 * Hierarchical Test:
 *  
 *   Registers a root scheduler, which then creates a hierarchy
 *   of schedulers. The hierarchy is NLEVELS deep; each scheduler
 *   at level N creates N-1 children schedulers and requests N-1
 *   hard threads.
 *
 *   The test verifies that the total number of schedulers that
 *   executed is correct. Furthermore, each scheduler verifies that 
 *   the correct number of children registered and unregistered.
 *
 *   This is a stress test for handling multiple levels in the 
 *   scheduler hierarchy, as well as a large number of schedulers
 *   and a large number of back-to-back entering & yielding of 
 *   hard threads.
 *
 * Interface Tested: 
 *
 *   lithe_sched_register  lithe_sched_unregister  lithe_sched_request  
 *   lithe_sched_enter     lithe_sched_yield
 */

#include <stdlib.h>
#include <assert.h>

#include "lithe.h"
#include "atomic.h"
#include "lithe_test.h"

const int NLEVELS = 2;

int total = 0;

typedef struct {
  int nchildren;
  int ncreated;
  int nregs;
  int nunregs;
} foo_t;

void __foo(void *data);

void enter(void *this) 
{
  __foo((foo_t *) this);
  lithe_sched_yield();
}

void yield(void *this, lithe_sched_t *child)
{ 
  /* TODO(xoxo): once we can block, we can actually enter children */
  assert(0);
}

void reg(void *this, lithe_sched_t *child) 
{
  fetchadd(&((foo_t *) this)->nregs, 1);
}

void unreg(void *this, lithe_sched_t *child) 
{
  fetchadd(&((foo_t *) this)->nunregs, 1);
}

void request(void *this, lithe_sched_t *child, int k) 
{
  lithe_sched_request(k);
}

lithe_sched_funcs_t funcs = {
  .enter = enter,
  .yield = yield,
  .reg = reg,
  .unreg = unreg,
  .request = request,
  .unblock = unblock_nyi
};

void foo(int nchildren) 
{
  foo_t data = { nchildren, 0, 0, 0 };
  printf("foo(%d)\n", nchildren);
  lithe_sched_register(&funcs, (void *) &data, __foo, (void *) &data);
  assert(coherentread(&data.ncreated) >= data.nchildren);
  assert(coherentread(&data.nregs) == data.nchildren);
  assert(coherentread(&data.nunregs) == data.nchildren);
  fetchadd(&total, 1);
}

void __foo(void *data) 
{
  foo_t *this = (foo_t *) data;
  printf("__foo(%d)\n", this->nchildren);
  lithe_sched_request(this->nchildren);
  int ncreated = fetchadd(&this->ncreated, 1);
  while (ncreated < this->nchildren) {
    foo(this->nchildren - 1);
    ncreated = fetchadd(&this->ncreated, 1);
  }
  lithe_sched_unregister();
}

int nfoos()
{
  int i, j;
  int sum = 0;
  for (i = 0; i <= NLEVELS; i++) {
    int product = 1;
    for (j = 1; j <= NLEVELS; j++) {
      product *= ((j <= i) ? (NLEVELS - j + 1) : 1);
    }
    sum += product;
  }
  return sum;
}

int main() 
{
  lithe_initialize();
  foo(NLEVELS);
  assert(coherentread(&total) == nfoos());
  return EXIT_SUCCESS;
}




