#ifndef CHILD_H
#define CHILD_H


#include "spmd.h"
#include "lithe.h"
#include "spinlock.h"
#include "atomic.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>


/* TODO: Figure out more elegant & portable solution to
 * creating padded arrays. For now, assume that the L1 cache 
 * line is CACHE_LINE_SIZE bytes to avoid false sharing. */

#define CACHE_LINE_SIZE 64


typedef union {
  lithe_sched_t *val;
  uint8_t padding[CACHE_LINE_SIZE];  
} sched_t;


typedef struct {
  sched_t sched;
  int request;
  int mtx;
} child_t;


typedef union {
  child_t val;
  uint8_t padding[CACHE_LINE_SIZE];  
} padded_child_t;


typedef struct {
  padded_child_t *array;
  int len;
  
  int lastreq;
} children_t;


void children_create(children_t *children, int len) 
{
  assert(children != NULL);
  children->array = (padded_child_t *)calloc(len, sizeof(padded_child_t));
  children->len = len;
  children->lastreq = -1;
}


void children_destroy(children_t &children) 
{
  free(children.array);
}


lithe_sched_t * children_next(children_t &children) 
{
  child_t *child;
  lithe_sched_t *sched = NULL;
  for (int i = coherentread(&children.lastreq); i >= 0; i--) {
    child = &children.array[i].val;
    spinlock_lock(&child->mtx);
    if ((child->sched.val != NULL) && (child->request > 0)) {
      child->request -= 1;
      sched = child->sched.val;
      spinlock_unlock(&child->mtx);
      break;
    }
    spinlock_unlock(&child->mtx);
  }
  return sched;
}


void children_add(children_t &children, lithe_sched_t *__child) 
{
  child_t *child = &children.array[spmd_tid()].val;
  spinlock_lock(&child->mtx);
  assert(child->sched.val == NULL);
  child->sched.val = __child;
  child->request = 0;
  spinlock_unlock(&child->mtx);
}


void children_remove(children_t &children, lithe_sched_t *__child)
{
  child_t *child = &children.array[spmd_tid()].val;
  spinlock_lock(&child->mtx);
  assert(child->sched.val == __child);
  child->sched.val = NULL;
  spinlock_unlock(&child->mtx);
}


void children_update(children_t &children, lithe_sched_t *__child, int k) 
{
  child_t *child;
  for (int i = 0; i < children.len; i++) {
    child = &children.array[i].val;
    if (child->sched.val == __child) {
      spinlock_lock(&child->mtx);
      assert(child->sched.val == __child);
      child->request += k;
      spinlock_unlock(&child->mtx);
      children.lastreq = i;
      wrfence();
      break;
    }
  }
}


#endif  // CHILD_H








