#include "spmd.h"
#include "atomic.h"
#include <assert.h>
#include <sys/sysinfo.h>
#include <iostream>


using namespace std;


void increment(void *__dptr) 
{
  atomic_add((int *)__dptr, spmd_tid());
}


int main(int argc, char **argv) 
{
  int sum;
#if 1
  for (int nthreads = 1; nthreads <= get_nprocs() * 2; nthreads++) {
#else
  {
    int nthreads = get_nprocs() * 2;
#endif
    sum = 0;
#if 0
    spmd_spawn(nthreads, increment, (void *)&sum);
#else
    spmd_init(nthreads);
    spmd_spawn(increment, (void *)&sum);
    spmd_fini();
#endif
    assert(sum == (((nthreads - 1) * nthreads) / 2));
  }
  cout << "SUCCESS" << endl;
  return 0;
}
