#ifndef INCLUDED__AOSContextQueue_IsAvailable_HPP__
#define INCLUDED__AOSContextQueue_IsAvailable_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueInterface.hpp"
#include "ASync_CriticalSectionSpinLock.hpp"
#include "ASync_CriticalSection.hpp"

class AThread;

class AOS_BASE_API AOSContextQueue_IsAvailable : public AOSContextQueueInterface
{
public:
  /*!
  queueCount - number of queues to use, each queue has its own select thread
  **/
  AOSContextQueue_IsAvailable(AOSServices& services, size_t queueCount = 1);
  virtual ~AOSContextQueue_IsAvailable();
  
  // queue thread control
  void startQueues();
  void stopQueues();

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

protected:
  /*!
  Synchronized remove method used by threadproc to perform work
  **/
  AOSContext *_nextContext();

private:
  typedef std::list<AOSContext *> REQUESTS;

  //a_Structure of a queue and its own worker thread
  class QueueWithThread : public ABase
  {
  public:
    ASync_CriticalSection sync;
    REQUESTS queue;
    AThread *pthread;

    QueueWithThread();
    virtual ~QueueWithThread();
    
    //a_Creates a new thread for the queue but will auto-start it
    void startThread();
  };

  // Queue container access
  ASync_CriticalSectionSpinLock m_SynchObjectContextContainer;
  typedef std::vector<QueueWithThread> QUEUES;
  QUEUES m_Queues;

  // Tries to read HTTP header for a given AOSRequest
  static u4 _threadprocWorker(AThread&);

  // Queue and synch
  size_t m_AddCounter;
};

#endif // INCLUDED__AOSContextQueue_IsAvailable_HPP__
