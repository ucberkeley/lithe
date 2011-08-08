#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lithe.h"
#include "atomic.h"


const int COUNT = 8;
static int count = 0;


void child_enter(void *__this) 
{
  printf("child_enter\n");
  fetchadd((int *) &count, 1);
  lithe_yield();
}


void child_yield(void *__this, lithe_sched_t *child)
{ 
  assert(false);
}


void child_reg(void *__this, lithe_sched_t *child) 
{
  assert(false);
}


void child_unreg(void *__this, lithe_sched_t *child) 
{
  assert(false);
}


void child_request(void *__this, lithe_sched_t *child, int k) 
{
  assert(false);
}


void child_unblock(void *__this, lithe_task_t *task) 
{
  assert(false);
}


lithe_sched_funcs_t child_funcs = {
  .enter = child_enter,
  .yield = child_yield,
  .reg = child_reg,
  .unreg = child_unreg,
  .request = child_request,
  .unblock = child_unblock,
};


void child()
{
  printf("child()\n");
  lithe_register(&child_funcs, NULL);
  lithe_request(COUNT);
  printf("before while\n");
  while (coherentread(&count) < COUNT);
  printf("after while\n");
  lithe_unregister();
}


struct parent_sched {
  lithe_sched_t *child;
};


void parent_enter(void *__this) 
{
  lithe_sched_t *child = ((struct parent_sched *) __this)->child;
  if (child != NULL) {
    lithe_enter(child);
  }
}


void parent_yield(void *__this, lithe_sched_t *child)
{ 
  printf("parent_yield\n");
  lithe_yield();
}


void parent_reg(void *__this, lithe_sched_t *child) 
{
  ((struct parent_sched *) __this)->child = child;
  printf("child 0x%x registered\n", child);
}


void parent_unreg(void *__this, lithe_sched_t *child) 
{
  ((struct parent_sched *) __this)->child = NULL;
  printf("child 0x%x unregistered\n", child);
}


void parent_request(void *__this, lithe_sched_t *child, int k) 
{
  lithe_request(k);
}


void parent_unblock(void *__this, lithe_task_t *task) 
{
  assert(false);
}


lithe_sched_funcs_t parent_funcs = {
  .enter = parent_enter,
  .yield = parent_yield,
  .reg = parent_reg,
  .unreg = parent_unreg,
  .request = parent_request,
  .unblock = parent_unblock,
};


void parent()
{
  printf("parent()\n");
  struct parent_sched __this = { NULL };
  lithe_register(&parent_funcs, &__this);
  child();
  lithe_unregister();
}


int main() 
{
  parent();
  return 0;
}
