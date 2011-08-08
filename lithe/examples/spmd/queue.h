#ifndef QUEUE_H
#define QUEUE_H


#include "lithe.h"
#include "spinlock.h"
#include <stdlib.h>
#include <assert.h>


template <class T>
class Queue
{
 public:
  Queue(size_t __max);
  ~Queue();
 public:
  void push(T item);
  bool pop(T *itemptr);
 private:
  inline bool empty();
  inline bool full();
 private:
  T *queue;
  size_t head;
  size_t tail;
  size_t max;
  int mtx;
};


template <class T>
Queue<T>::Queue(size_t __max) :
  head(0),
  tail(0),
  max(__max)
{
  queue = (T *)malloc((max + 1) * sizeof(T));
  assert(queue != NULL);
  spinlock_init(&mtx);
}


template <class T>
Queue<T>::~Queue() 
{
  free(queue);
}


template <class T>
void Queue<T>::push(T item) 
{
  spinlock_lock(&mtx);
  assert(!full());
  queue[tail] = item;
  tail = ((tail == max) ? 0 : (tail + 1));
  assert(!empty());
  spinlock_unlock(&mtx);
}


template <class T>
bool Queue<T>::pop(T *itemptr) 
{
  bool popped = false;
  assert(itemptr != NULL);
  spinlock_lock(&mtx);
  if (!empty()) {
    popped = true;
    *itemptr = queue[head];
    head = ((head == max) ? 0 : (head + 1));
  }
  spinlock_unlock(&mtx);
  return popped;
}


template <class T>
bool Queue<T>::empty() 
{
  return (head == tail);
}


template <class T>
bool Queue<T>::full() 
{
  if (tail >= head) {
    return ((head == 0) && (tail == max));
  } else {
    return ((head - tail) == 1);
  }
}


#endif  // QUEUE_H
