#if FUTURE_PTHREAD==1
#include "future_pthread.h"
#elif FUTURE_SPIN==1
#include "future_spin.h"
#else
#include "future.h"
#endif


#include "spmd.h"
#include "atomic.h"
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <sys/sysinfo.h>
#include <iostream>


using namespace std;


void delay() 
{
  for (uint64_t i = 0; i < 0x10000000; i++) {
    uint64_t iters = 0xffffffffffffffff;
    asm volatile ("1: mov %0, %%rax"
		  : /* no outputs */
		  : "r" (iters)
		  : "%rax" );
    asm volatile ("dec %rax");
    asm volatile ("cmp %rax, %rax");
    asm volatile ("jne 1b");
  }
}


void cascade(void *__fptrs) 
{
  int tid = spmd_tid();
  int val;
  future_ptr<int> *fptrs = (future_ptr<int> *)__fptrs;
  future_ptr<int> *inptr = &fptrs[tid + 1];
  future_ptr<int> *outptr = &fptrs[tid];
  assert(inptr->read() == (tid + 1));
  delay();
  val = tid;
  outptr->write(val); 
}


int main(int argc, char **argv) 
{
  int nthreads;
  int *inputs;
  future_ptr<int> *fptrs;
  
#if 0
  for (nthreads = get_nprocs(); nthreads <= get_nprocs() * 16; nthreads *= 2) {
#else
  {
    nthreads = 16;
#endif

    cout << nthreads << " THREAD(S)" << endl;

    inputs = new int[nthreads + 1];
    fptrs = new future_ptr<int>[nthreads + 1]; 
     
    for (int i = 0; i <= nthreads; i++) {
      fptrs[i].init(&inputs[i]);
    }
    fptrs[nthreads].write(nthreads);

    spmd_spawn(nthreads, cascade, (void *)fptrs);

    for (int i = 0; i <= nthreads; i++) {
      assert(inputs[i] == i);
      assert(fptrs[i].read() == i);
    }
    
    delete [] inputs;
    delete [] fptrs;
  }
  cout << "SUCCESS" << endl;
  return 0;
}
