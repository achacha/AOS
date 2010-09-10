/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSContextQueueThreadPool_RoundRobinSwarm_HPP__
#define INCLUDED__AOSContextQueueThreadPool_RoundRobinSwarm_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AThreadPool.hpp"
#include "ABasePtrQueue.hpp"

class AOSServices;
class AOSContext;
class AOSContextQueueSet;

class AOS_BASE_API AOSContextQueueThreadPool_RoundRobinSwarm : public AOSContextQueueInterface
{
public:
  //! Class name
  static const AString CLASS;
  
  //! Default sleep delay time
  static const int DEFAULT_SLEEP_DELAY;

public:
  /*!
  ctor
  */
  AOSContextQueueThreadPool_RoundRobinSwarm(
    AOSServices&,  
    size_t threadCount = 16,
    size_t queueCount = 4
  );
  virtual ~AOSContextQueueThreadPool_RoundRobinSwarm();

  /*!
  Externally add AOSContext* synchronously to this queue for processing
  */
  virtual void add(AOSContext *);

  /*!
  Access to the thread pool

  @return reference to AThreadPool
  */
  AThreadPool& useThreadPool() { return m_ThreadPool; }

  /*!
  Check if queue is running

  @return true if all queues/threads are running
  */
  virtual bool isRunning();

  /*!
  Sleep delay
  */
  void setSleepDelay(int sleepDelay);
  int getSleepDelay() const;

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

protected:
  /*!
  Synchronized remove method used by threadproc to perform work
  */
  virtual AOSContext *_nextContext();

  /*!
  Overridden by child class to add behavior to this queue
  */
  virtual u4 _threadproc(AThread&) = 0;

  // The tread pool
  AThreadPool m_ThreadPool;

  // Main wrapper to the thread pool workers
  static u4 _threadprocWrapperRoundRobbin(AThread&);

  //a_Thread properties
  int m_SleepDelay;

private:
  //a_AOSContext container
  typedef std::vector<ABasePtrQueue *> QUEUES;
  QUEUES m_Queues;

  typedef std::vector<size_t> ADD_COUNTERS;
  ADD_COUNTERS m_AddCounters;

  const size_t m_queueCount;
  volatile long m_currentWriteQueue;
  volatile long m_currentReadQueue;
};

#endif //INCLUDED__AOSContextQueueThreadPool_RoundRobinSwarm_HPP__
