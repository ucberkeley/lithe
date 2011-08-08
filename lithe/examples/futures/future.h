#ifndef FUTURE_H
#define FUTURE_H


#include "lithe.h"
#include "spinlock.h"
#include <vector>
#include <assert.h>


using namespace std;


template <class T>
class future_ptr 
{
 public:
  future_ptr(T *__dptr = NULL);
  ~future_ptr();
 public:
  void init(T *__dptr);
  T & read();
  void write(T &val);
 public: 
  template<class U> friend void __block(lithe_task_t *task, void *__this);
 protected: 
  void block(lithe_task_t *task);
 private:
  T *dptr;
  bool valid;
 private:
  int mtx;
  vector<lithe_task_t *> blocked;
};


template<class T>
void __block(lithe_task_t *task, void *__this) 
{
  future_ptr<T> *fptr = (future_ptr<T> *)__this;
  assert(fptr != NULL);
  fptr->block(task);
}


template<class T>
future_ptr<T>::future_ptr(T *__dptr) :
  dptr(__dptr),
  valid(false),
  mtx(0)
{}


template<class T>
void future_ptr<T>::init(T *__dptr)  
{
  dptr = __dptr;
}


template<class T>
future_ptr<T>::~future_ptr()
{}


template<class T>
T & future_ptr<T>::read()
{
  spinlock_lock(&mtx);
  if (!valid) {
    lithe_task_block(__block<T>, this);
  } else {
    spinlock_unlock(&mtx);
  }
  return *dptr;
}


template<class T>
void future_ptr<T>::write(T &val) 
{
  *dptr = val;
  spinlock_lock(&mtx);
  valid = true;
  vector<lithe_task_t *>::iterator itr; 
  for (itr = blocked.begin(); itr != blocked.end(); itr++) {
    lithe_task_unblock(*itr);
  }
  blocked.clear();
  spinlock_unlock(&mtx);
}


template<class T>
void future_ptr<T>::block(lithe_task_t *task) 
{
  blocked.push_back(task);
  spinlock_unlock(&mtx);
  lithe_sched_reenter();
}


#endif  // FUTURE_H
