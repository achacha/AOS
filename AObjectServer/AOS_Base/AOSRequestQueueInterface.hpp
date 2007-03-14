#ifndef INCLUDED__AOSRequestQueueInterface_HPP__
#define INCLUDED__AOSRequestQueueInterface_HPP__

#include "apiAOS_Base.hpp"
#include "ACriticalSection.hpp"

class AOSRequest;
class AOSRequestQueueSet;
class AOSServices;

class AOS_BASE_API AOSRequestQueueInterface
{
public:
  /*!
  pForward - Forward queue set for AOSRequest* (if applicable)
  pBack - Go back (if applicable)
  **/
  AOSRequestQueueInterface(
    AOSServices& services,
    AOSRequestQueueInterface *pForward = NULL, 
    AOSRequestQueueInterface *pBack = NULL
  );
  virtual ~AOSRequestQueueInterface();
    
  /*!
  Externally add AOSRequest* synchronously to this queue for processing
  **/
  virtual void add(AOSRequest *) = 0;

  /*!
  Set queues
  **/
  void setForwardQueueSet(AOSRequestQueueInterface *);
  void setBackQueueSet(AOSRequestQueueInterface *);

protected:
  /*!
  Synchronization object
  */
  ACriticalSection m_SynchObject;

  /*!
  Queue sets for chaining
  */
  AOSRequestQueueInterface *mp_Forward;
  AOSRequestQueueInterface *mp_Back;

  /*!
  Synchronized remove method used by threadproc to perform work
  **/
  virtual AOSRequest *_nextRequest() = 0;

  /*!
  Chaining AOSRequest*
  if chained queue is NULL, AOSRequest::dealloc() is called
  **/
  virtual void _goForward(AOSRequest *);
  virtual void _goBack(AOSRequest *);

  /*!
  AOS services
  */
  AOSServices& m_AOSServices;
};

#endif // INCLUDED__AOSRequestQueueInterface_HPP__