/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSContextQueue_IsAvailable_HPP__
#define INCLUDED__AOSContextQueue_IsAvailable_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueInterface.hpp"
#include "ASync_CriticalSectionSpinLock.hpp"
#include "ASync_CriticalSection.hpp"
#include "ABasePtrQueue.hpp"

class AThread;

class AOS_BASE_API AOSContextQueue_IsAvailable : public AOSContextQueueInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  /*!
  queueCount - number of queues to use, each queue has its own select thread
  **/
  AOSContextQueue_IsAvailable(AOSServices& services, size_t queueCount = 1);
  virtual ~AOSContextQueue_IsAvailable();
  
  //! Start
  virtual void start();
  
  //! Stop
  virtual void stop();

  /*!
  Externally add AOSContext* synchronously to this queue for processing
  **/
  virtual void add(AOSContext *);

  /*!
  Admin interface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  Synchronized remove method used by threadproc to perform work
  **/
  AOSContext *_nextContext();

private:
  //a_Structure of a queue and its own worker thread
  class QueueWithThread : public ABase
  {
  public:
    ASync_CriticalSection sync;
    ABasePtrQueue queue;
    AThread *pthread;
    size_t count;      //a_Usage meter

    QueueWithThread();
    virtual ~QueueWithThread();
  };

  // Queue container access
  ASync_CriticalSectionSpinLock m_SynchObjectContextContainer;
  typedef std::vector<QueueWithThread *> QUEUES;
  QUEUES m_Queues;
  size_t m_queueCount;

  // Tries to read HTTP header for a given AOSRequest
  static u4 _threadprocWorker(AThread&);

  // Queue and synch
  volatile long m_currentQueue;

  // Sleep times
  int m_NoDataTimeout;
  int m_EmptyQueueDelay;
  int m_LoopDelay;
};

#endif // INCLUDED__AOSContextQueue_IsAvailable_HPP__
