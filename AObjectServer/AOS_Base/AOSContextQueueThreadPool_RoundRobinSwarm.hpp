#ifndef INCLUDED__AOSContextQueueThreadPool_RoundRobinSwarm_HPP__
#define INCLUDED__AOSContextQueueThreadPool_RoundRobinSwarm_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AOSContextQueueThreadPool.hpp"

class AOSRequest;
class AOSContext;

class AOS_BASE_API AOSContextQueueThreadPool_RoundRobinSwarm : public AOSContextQueueThreadPool
{
public:
  AOSContextQueueThreadPool_RoundRobinSwarm(
    AOSServices&,  
    size_t threadCount = 16,
    size_t queueCount = 4,
    AOSContextQueueInterface *pYes = NULL, 
    AOSContextQueueInterface *pNo = NULL,
    AOSContextQueueInterface *pError = NULL
  );
  virtual ~AOSContextQueueThreadPool_RoundRobinSwarm();

  /*!
  Externally add AOSContext* synchronously to this queue for processing
  **/
  virtual void add(AOSContext *);

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

protected:
  /*!
  Synchronized remove method used by threadproc to perform work
  **/
  virtual AOSContext *_nextContext();

private:
  //a_AOSContext container
  typedef std::vector<ASynchronization *> QUEUE_LOCKS;
  QUEUE_LOCKS m_QueueLocks;

  typedef std::deque<AOSContext *> QUEUE;
  typedef std::vector<QUEUE> QUEUES; 
  QUEUES m_Queues;

  typedef std::vector<size_t> ADD_COUNTERS;
  ADD_COUNTERS m_AddCounters;

  const size_t m_queueCount;
  volatile long m_currentWriteQueue;
  volatile long m_currentReadQueue;
};

#endif //INCLUDED__AOSContextQueueThreadPool_RoundRobinSwarm_HPP__
