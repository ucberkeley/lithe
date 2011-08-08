#ifndef FUTURE_SPIN_H
#define FUTURE_SPIN_H


#include "atomic.h"
#include <assert.h>
#include <stdlib.h>


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
};


template<class T>
future_ptr<T>::future_ptr(T *__dptr) :
  dptr(__dptr),
  valid(false) 
{}


template<class T>
future_ptr<T>::~future_ptr()
{}


template<class T>
void future_ptr<T>::init(T *__dptr)  
{
  dptr = __dptr;
}


template<class T>
T & future_ptr<T>::read() 
{
  while (!coherentread(&valid)) {}
  return *dptr;
}


template<class T>
void future_ptr<T>::write(T &val) 
{
  *dptr = val;
  valid = true;
}


#endif  // FUTURE_SPIN_H





