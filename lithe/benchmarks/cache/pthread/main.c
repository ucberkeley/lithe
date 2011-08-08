/**
 * Force pthreads to compete for L1. 
 * 
 * Assuming a 64 byte aligned memory region, addresses conflict in the
 * L1 at: addr + 0, addr + 32768, addr + (i * SETS * LINESIZE)
*/

#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>

#include "fatal.h"
#include "pthread_np.h"

#include "main.h"

#define CPUID 4


void * __module0(void *arg)
{
  void *address = arg;

  for (int line = 0; line < 1024; line++) {
    char *c = address + (line * LINESIZE);
    *c = (char) line;
  }

  for (int i = 0; i < ITERATIONS; i++) {
    for (int line = 0; line < 1024; line++) {
      char *c = address + (line * LINESIZE);
      if (*c != (char) line) {
	fatal("bad cache line");
      }
    }
    pthread_yield();
  }
  return NULL;
}


void * module0(void *arg) {
  pthread_t thrd;
  if (pthread_create_pinned_np(CPUID, &thrd, NULL, __module0, arg) != 0) {
    fatal("failed to create pinned thread");
  }

  if (pthread_join(thrd, NULL) != 0) {
    fatal("failed to join with thread");
  }

  return NULL;
}


void * __module1(void *arg)
{
  void *address = arg;

  for (int line = 0; line < 1024; line++) {
    char *c = address + (line * LINESIZE);
    *c = (char) line;
  }

  for (int i = 0; i < ITERATIONS; i++) {
    for (int line = 0; line < 1024; line++) {
      char *c = address + (line * LINESIZE);
      if (*c != (char) line) {
	fatal("bad cache line");
      }
    }
    pthread_yield();
  }
  return NULL;
}


void * module1(void *arg) {
  pthread_t thrd;
  if (pthread_create_pinned_np(CPUID, &thrd, NULL, __module1, arg) != 0) {
    fatal("failed to create pinned thread");
  }

  if (pthread_join(thrd, NULL) != 0) {
    fatal("failed to join with thread");
  }

  return NULL;
}


void module()
{
  /* Allocate capacity of L1 (1024 cache lines) for each thread. */
  // TODO(benh): Try allocating one big chunk rather than two chunks. */
  void *addrs[2];
  if (posix_memalign(&addrs[0], 64, SETS * WAYS * LINESIZE) != 0 ||
      posix_memalign(&addrs[1], 64, SETS * WAYS * LINESIZE) != 0) {
    fatal("failed to allocate memory");
  }

  pthread_t thrds[2];
  if (pthread_create_pinned_np(0, &thrds[0], NULL, module0, addrs[0]) != 0 ||
      pthread_create_pinned_np(1, &thrds[1], NULL, module1, addrs[1]) != 0) {
    fatal("failed to create pinned threads");
  }

  if (pthread_join(thrds[0], NULL) != 0 ||
      pthread_join(thrds[1], NULL) != 0) {
    fatal("failed to join with threads");
  }
}


int main(int argc, char **argv)
{
  module();
  return 0;
}
