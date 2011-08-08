#ifndef SPIN_BARRIER_H
#define SPIN_BARRIER_H


#include <stdint.h>


/* TODO: Figure out more elegant & portable solution to
 * creating padded arrays. For now, assume that the L1 cache 
 * line is CACHE_LINE_SIZE bytes to avoid false sharing. */

#define CACHE_LINE_SIZE 64


typedef union {
  bool val;
  uint64_t padding[CACHE_LINE_SIZE];
} padded_bool_t;


typedef struct {
  int N;
  int arrived;
  padded_bool_t *signals;
  bool wait;
} spin_barrier_t;


void spin_barrier_init(spin_barrier_t *barrier, int nthreads);
void spin_barrier_destroy(spin_barrier_t *barrier);
void spin_barrier_wait(spin_barrier_t *barrier);


#endif
