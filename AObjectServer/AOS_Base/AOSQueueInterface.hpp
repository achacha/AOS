#ifndef INCLUDED__AOSQueueInterface_HPP__
#define INCLUDED__AOSQueueInterface_HPP__

#include "apiAOS_Base.hpp"
#include "AOSQueueSet.hpp"

template <class T>
class AOS_BASE_API AOSQueueInterface
{
public:
  /*!
  pForward - Forward queue set for AOSRequest* (if applicable)
  pBack - Go back (if applicable)
  **/
  AOSContextQueueInterface(
    AOSQueueSet<T> *pForward = NULL, 
    AOSQueueSet<T> *pBack = NULL
  ) :
    mp_Forward(pForward),
    mp_Back(pBack)
  {
  }
  virtual ~AOSContextQueueInterface() {}
    
  /*!
  Externally add AOSRequest* synchronously to this queue for processing
  **/
  virtual void add(T *) = 0;

protected:
  /*!
  Queue sets for chaining
  */
  AOSQueueSet<T> *mp_Forward;
  AOSQueueSet<T> *mp_Back;

  /*!
  Synchronized remove method used by threadproc to perform work
  **/
  virtual T *_next() = 0;

  /*!
  Chaining queues
  if chained queue is NULL, T::deallocate() is called
  **/
  virtual void _goForward(T *);
  virtual void _goBack(T *);
};

#endif // INCLUDED__AOSQueueInterface_HPP__
