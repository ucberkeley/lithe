#include "spmd.h"
#include <pthread.h>
#include <assert.h>


__thread int id;
__thread int nthreads;


typedef struct {
  int id;
  void (*func)(void *);
  void *arg;
} arg_t;


#include <iostream>
using namespace std;


void * __spmd_spawn(void *__args) 
{
  arg_t *args = (arg_t *)__args;
  assert(args != NULL);
  id = args->id;
  args->func(args->arg);
  return 0;
}


void spmd_spawn(void (*__func)(void *), void *__arg) 
{
  pthread_t threads[nthreads];
  arg_t args[nthreads];
  assert(nthreads > 0);
  for (int i = 0; i < nthreads; i++) {
    args[i].id = i;
    args[i].func = __func;
    args[i].arg = __arg;
    pthread_create(&threads[i], NULL, __spmd_spawn, (void *)&args[i]);
  }
  for (int i = 0; i < nthreads; i++) {
    pthread_join(threads[i], NULL);
  }
}


void spmd_spawn(int __nthreads, void (*__func)(void *), void *__arg) 
{
  nthreads = __nthreads;
  spmd_spawn(__func, __arg);
}


void spmd_init(int __nthreads) 
{
  nthreads = __nthreads;
}


void spmd_fini()
{}


int spmd_tid() 
{
  return id;
}


spmd_key_t spmd_tls_alloc() 
{
  pthread_key_t key;
  assert(pthread_key_create(&key, NULL) == 0);
  return (spmd_key_t)key;
}


void * spmd_tls_get(spmd_key_t key) 
{
  return pthread_getspecific((pthread_key_t)key);
}


void spmd_tls_set(spmd_key_t key, void *val) 
{
  pthread_setspecific((pthread_key_t)key, val);
}








  
  



