#ifndef FUTURE_PTHREAD_H
#define FUTURE_PTHREAD_H


#include <pthread.h>
#include <assert.h>


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
 private:
  T *dptr;
  bool valid;
 private:
  pthread_cond_t cond;
  pthread_mutex_t mtx;
};


template<class T>
future_ptr<T>::future_ptr(T *__dptr) :
  dptr(__dptr),
  valid(false)
{
  pthread_cond_init(&cond, NULL);
  pthread_mutex_init(&mtx, NULL);
}


template<class T>
future_ptr<T>::~future_ptr()
{
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mtx);
}


template<class T>
void future_ptr<T>::init(T *__dptr)  
{
  dptr = __dptr;
}


template<class T>
T & future_ptr<T>::read() 
{
  pthread_mutex_lock(&mtx);
  while (!valid) {
    pthread_cond_wait(&cond, &mtx);
  }
  pthread_mutex_unlock(&mtx);
  return *dptr;
}


template<class T>
void future_ptr<T>::write(T &val) 
{
  *dptr = val;
  pthread_mutex_lock(&mtx);
  valid = true;
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mtx);
}


#endif  // FUTURE_PTHREAD_H
