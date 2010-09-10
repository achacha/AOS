/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSContextQueueInterface_HPP__
#define INCLUDED__AOSContextQueueInterface_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ASync_CriticalSectionSpinLock.hpp"

class AOSServices;
class AOSContext;

class AOS_BASE_API AOSContextQueueInterface : public AOSAdminInterface
{
public:
  /*!
  ctor

  @param services reference to AOSServices object
  */
  AOSContextQueueInterface(AOSServices& services);
  
  //! dtor
  virtual ~AOSContextQueueInterface();
    
  /*!
  Externally add AOSContext* synchronously to this queue for processing

  @param p AOSContext to add
  */
  virtual void add(AOSContext *p) = 0;

  /*!
  Start the queue
  */
  virtual void start() = 0;

  /*!
  Stop the queue
  */
  virtual void stop() = 0;

  /*!
  Check if queue is running

  @return true if all queues/threads are running
  */
  virtual bool isRunning() = 0;

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);

protected:
  /*!
  AOSServices
  */
  AOSServices& m_Services;

  /*!
  Synchronized remove method used by threadproc to perform work
  **/
  virtual AOSContext *_nextContext() = 0;
};

#endif // INCLUDED__AOSContextQueueInterface_HPP__
