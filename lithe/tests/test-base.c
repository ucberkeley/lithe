#include <ht.h>
#include <stdio.h>
#include <unistd.h>

#include "lithe.h"
#include "fatal.h"


static void reg(void *__this, lithe_sched_t *sched)
{
  fatal("unimplemented");
}


static void unreg(void *__this, lithe_sched_t *sched)
{
  fatal("unimplemented");
}


static void request(void *__this, lithe_sched_t *sched, int k)
{
  fatal("unimplemented");
}


static void enter(void *__this)
{
  printf("enter [%d]\n", ht_id());
  lithe_yield();
}


static void yield(void *__this)
{
  fatal("unimplemented");
}


static void unblock(void *__this, lithe_task_t *task)
{
  fatal("unimplemented");
}


static const lithe_sched_funcs_t funcs = {
  .enter   = enter,
  .yield   = yield,
  .reg     = reg,
  .unreg   = unreg,
  .request = request,
  .unblock = unblock,
};


int main(int argc, char **argv)
{
  lithe_register(&funcs, NULL);
  printf("main [%d]\n", ht_id());
  lithe_request(2);
  sleep(2);
  lithe_unregister();
  return 0;
}
