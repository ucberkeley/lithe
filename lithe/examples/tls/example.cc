#include "spmd.h"
#include "atomic.h"
#include <vector>
#include <assert.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <iostream>


using namespace std;


int sum = 0;
int MAXKEYS = 16;
vector<spmd_key_t> keys(MAXKEYS, 0);


void compute(void *__nkeys) 
{
  int tid = spmd_tid();
  int nkeys = (int)(long)__nkeys;
  int addend = 0;
  for (int i = 0; i < nkeys; i++) {
    spmd_tls_set(keys.at(i), (void *)(tid * (i + 1))); 
  }
  for (int i = 0; i < nkeys; i++) {
    addend += (int)(long)spmd_tls_get(keys.at(i));
  }
  assert(addend == (((1 + nkeys) * nkeys / 2) * tid));
  atomic_add(&sum, addend);
}


int main(int argc, char **argv) 
{
  for (int nthreads = 1; nthreads <= get_nprocs() * 2; nthreads *= 2) {
    for (int nkeys = 1; nkeys <= MAXKEYS; nkeys *= 2) {
      cout << "NTHREADS = " << nthreads << ", NKEYS = " << nkeys << endl;
      sum = 0;
      spmd_init(nthreads);
      for (int i = 0; i < MAXKEYS; i++) {
	keys.at(i) = spmd_tls_alloc();
      }
      spmd_spawn(compute, (void *)nkeys);
      spmd_fini();
      assert(sum == ((((nthreads - 1) * nthreads) / 2) * ((1 + nkeys) * nkeys / 2)));
    }
  }
  cout << "SUCCESS" << endl;
  return 0;
}
