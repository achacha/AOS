#ifndef INCLUDED__AOSRequestQueue_IsAvailable_HPP__
#define INCLUDED__AOSRequestQueue_IsAvailable_HPP__

#include "apiAOS_Base.hpp"
#include "AOSRequestQueueThreadPool.hpp"

class AOS_BASE_API AOSRequestQueue_IsAvailable : public AOSRequestQueueThreadPool
{
public:
  /*!
  thread count for this queue is always 1
  pForward - Forward queue set for AOSRequest* (if applicable)
  pBack - Go back (if applicable)
  **/
  AOSRequestQueue_IsAvailable(
    AOSServices& services,
    AOSRequestQueueInterface *pForward = NULL, 
    AOSRequestQueueInterface *pBack = NULL
  );
  virtual ~AOSRequestQueue_IsAvailable();
  
  /*!
  Externally add AOSRequest* synchronously to this queue for processing
  **/
  virtual void add(AOSRequest *);

protected:
  /*!
  Tries to read HTTP header for a given AOSRequest
  **/
  virtual u4 _threadproc(AThread&);

  /*!
  Synchronized remove method used by threadproc to perform work
  **/
  AOSRequest *_nextRequest();

  /*!
  Chaining AOSRequest*
  if chained queue is NULL, AOSRequest::dealloc() is called
  **/
//  void _goForward(AOSRequest *);
//  void _goBack(AOSRequest *);

private:
  // Queue and synch
  typedef std::list<AOSRequest *> REQUESTS;
  REQUESTS m_Queue;
  ACriticalSection m_SynchObject;
};

#endif // INCLUDED__AOSRequestQueue_IsAvailable_HPP__
