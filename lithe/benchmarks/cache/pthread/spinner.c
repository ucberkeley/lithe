#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>


static void spinner(pthread_mutex_t *mutex, int hold)
{
  while (1) {
    pthread_mutex_lock(mutex);
    if (hold) sleep(1);
    pthread_mutex_unlock(mutex);
  }
}

/* Test maximum time to lock given furious spinners. */
int main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <numspinners> <iterations>\n", argv[0]);
    exit(1);
  }

  pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex, NULL);

  pid_t children[100];
  for (unsigned int i = 0; i < atoi(argv[1]); i++) {
    children[i] = fork();
    if (children[i] == 0)
      // spinner(mutex, i < atoi(argv[1])/2);
      spinner(mutex, 1);
  }

  unsigned long maxtime = 0;

  for (unsigned int i = 0; i < atoi(argv[2]); i++) {
    struct timeval start, end, diff;

    sleep(1);
    gettimeofday(&start, NULL);
    pthread_mutex_lock(mutex);
    gettimeofday(&end, NULL);
    pthread_mutex_unlock(mutex);
    timersub(&end, &start, &diff);
    printf("Wait time: %lu.%06lu\n", diff.tv_sec, diff.tv_usec);
    if (diff.tv_sec * 1000000 + diff.tv_usec > maxtime)
      maxtime = diff.tv_sec * 1000000 + diff.tv_usec;
  }

  /* Kill children */
  for (unsigned i = 0; i < atoi(argv[1]); i++)
    kill(children[i], SIGTERM);

  printf("Worst case: %lu\n", maxtime);
  exit(0);
}
