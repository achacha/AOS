/*
Written by Alex Chachanashvili

Id: $Id$
*/
#ifndef INCLUDED__AOSContextQueueThreadPool_RoundRobinSwarm_HPP__
#define INCLUDED__AOSContextQueueThreadPool_RoundRobinSwarm_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AOSContextQueueThreadPool.hpp"
#include "ABasePtrQueue.hpp"

class AOSContext;

class AOS_BASE_API AOSContextQueueThreadPool_RoundRobinSwarm : public AOSContextQueueThreadPool
{
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
