/*
 * Simple Test:
 * 
 *   Registers a root scheduler which requests the maximum number 
 *   of hard threads, then verifies that the requested number of
 *   enters actually occur.
 *
 * Interface Tested: 
 *
 *   lithe_register  lithe_unregister  lithe_request  
 *   lithe_enter     lithe_yield       
*/

#include <stdlib.h>
#include <assert.h>

#include "lithe.h"
#include "atomic.h"
#include "lithe_test.h"


void enter(void *counter)
{
  fetchadd((int *) counter, 1);
  lithe_sched_yield();
}


lithe_sched_funcs_t funcs = {
  .enter = enter,
  .yield = yield_nyi,
  .reg = reg_nyi,
  .unreg = unreg_nyi,
  .request = request_nyi,
  .unblock = unblock_nyi
};


void start(void *counter)
{
  int limit = ht_limit_hard_threads();
  lithe_sched_request(limit - 1);
  while (coherentread((int *) counter) < limit - 1);
  lithe_sched_unregister();
}


int main()
{
  lithe_initialize();
  int counter = 0;
  lithe_sched_register(&funcs, (void *) &counter, start, (void *) &counter);
  return EXIT_SUCCESS;
}
