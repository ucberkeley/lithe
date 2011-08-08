/**
 * Force pthreads to compete for L1. 
 * 
 * Assuming a 64 byte aligned memory region, addresses conflict in the
 * L1 at: addr + 0, addr + 32768, addr + (i * SETS * LINESIZE)
*/

#define _GNU_SOURCE
#include <stdlib.h>

#include "fatal.h"
#include "lithe.h"

#include "main.h"


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
  }
  return NULL;
}


void * module0(void *arg) {
  __module0(arg);
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
  }
  return NULL;
}


void * module1(void *arg) {
  __module1(arg);
  return NULL;
}


void reg(void *this, struct lithe_sched *sched)
{
  fatal("unimplemented");
}


void unreg(void *this, struct lithe_sched *sched)
{
  fatal("unimplemented");
}


void request(void *this, struct lithe_sched *sched, int k)
{
  fatal("unimplemented");
}


void enter(void *this)
{
  fatal("unimplemented");
}


void yield(void *this)
{
  fatal("unimplemented");
}


void block(void *this, ucontext_t *ctx, void *args)
{
  fatal("unimplemented");
}


static const struct lithe_sched_funcs funcs = {
  .reg     = reg,
  .unreg   = unreg,
  .request = request,
  .enter   = enter,
  .yield   = yield,
  .block   = block,
};


void module()
{
  /* Allocate capacity of L1 (1024 cache lines) for each thread. */
  // TODO(benh): Try allocating one big chunk rather than two chunks. */
  void *addrs[2];
  if (posix_memalign(&addrs[0], 64, SETS * WAYS * LINESIZE) != 0 ||
      posix_memalign(&addrs[1], 64, SETS * WAYS * LINESIZE) != 0) {
    fatal("failed to allocate memory");
  }

  lithe_register(&funcs, NULL);
  /* lithe_request(2); */
  module0(addrs[0]);
  module1(addrs[1]);
  lithe_unregister();
}


int main(int argc, char **argv)
{
  module();
  return 0;
}
