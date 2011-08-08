#ifndef TASK_H
#define TASK_H


#include "lithe.h"
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>


const size_t DEFAULT_STACK_SIZE = getpagesize() * 8;


lithe_task_t * task_create(size_t stacksize = DEFAULT_STACK_SIZE) 
{
  /* TODO: is it fair for us to assume that the stack_t 
   * variable won't live past the lithe_task_init call? */
  stack_t stack; 
  stack.ss_size = stacksize;
  stack.ss_sp = malloc(stacksize);

  lithe_task_t *task = (lithe_task_t *) malloc(sizeof(lithe_task_t));
  lithe_task_init(task, &stack);
  return task;
}


void task_destroy(lithe_task_t *task) 
{
  /* TODO: fix interface, need better way to get back stack */ 
  assert(task != NULL);
  free(task->ctx.uc_stack.ss_sp);
  free(task);
}


#endif  // TASK_H
