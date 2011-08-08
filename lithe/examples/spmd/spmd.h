#ifndef SPMD_H
#define SPMD_H


/* SPMD Scheduler                                  */
/* Single-Program Multiple-Data programming model. */


/* Spawn "nthreads" number of threads, each starting to execute
 * func(arg). This will only return after all threads have
 * completed. Each thread can be assigned a different computation
 * based on its unique thread ID. */

void spmd_spawn(int nthreads, void (*func)(void *), void *arg);


/* Alternate interface to split up the init/fini and spawn */

void spmd_spawn(void (*func)(void *), void *arg);
void spmd_init(int nthreads);
void spmd_fini();


/* Return the thread id of the currently executing thread 
 * (between 0 and (nthreads - 1). */

int spmd_tid();


/* Thread-local storage 
 * TODO(xoxo): need a destructor 
 * TODO(xoxo): should we add this to the general lithe interface?
 */    

typedef int spmd_key_t;

spmd_key_t spmd_tls_alloc();
void * spmd_tls_get(spmd_key_t key);
void spmd_tls_set(spmd_key_t key, void *val);


#endif  // SPMD_H
