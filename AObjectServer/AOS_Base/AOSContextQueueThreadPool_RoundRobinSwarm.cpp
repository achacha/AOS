#include "pchAOS_Base.hpp"
#include "AOSContextQueueThreadPool_RoundRobinSwarm.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "AOSCommand.hpp"
#include "AFileSystem.hpp"
#include "AFile_Physical.hpp"
#include "AZlib.hpp"
#include "AOSConfiguration.hpp"

const AString& AOSContextQueueThreadPool_RoundRobinSwarm::getClass() const
{
  static const AString CLASS("AOSContextQueueThreadPool_RoundRobinSwarm");
  return CLASS;
}

void AOSContextQueueThreadPool_RoundRobinSwarm::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueThreadPool::addAdminXml(eBase, request);

  addProperty(
    eBase,
    ASW("CurrentReadQueue",16),
    AString::fromInt(m_currentReadQueue % m_queueCount)
  );

  addProperty(
    eBase,
    ASW("CurrentWriteQueue",17),
    AString::fromInt(m_currentWriteQueue % m_queueCount)
  );

  addProperty(
    eBase,
    ASW("Queues.size",11),
    AString::fromSize_t(m_Queues.size())
  );

  for (size_t i=0; i<m_AddCounters.size(); ++i)
  {
    addProperty(
      eBase,
      ARope("Queue[")+AString::fromSize_t(i)+ASW("].count",7),
      AString::fromSize_t(m_AddCounters[i])
    );

    addProperty(
      eBase,
      ARope("Queue[")+AString::fromSize_t(i)+ASW("].size",6),
      AString::fromSize_t(m_Queues[i].size())
    );
  }
}

void AOSContextQueueThreadPool_RoundRobinSwarm::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueThreadPool::processAdminAction(eBase, request);
}

AOSContextQueueThreadPool_RoundRobinSwarm::AOSContextQueueThreadPool_RoundRobinSwarm(
  AOSServices& services,
  size_t threadCount,              // = 16
  size_t queueCount,               // = 4
  AOSContextQueueInterface *pYes,  // = NULL 
  AOSContextQueueInterface *pNo,   // = NULL
  AOSContextQueueInterface *pError // = NULL
) :
  m_queueCount(queueCount),
  AOSContextQueueThreadPool(services, threadCount, pYes, pNo, pError),
  m_currentWriteQueue(0),
  m_currentReadQueue(queueCount-1)
{
  AASSERT(NULL, queueCount > 0);
  m_Queues.resize(m_queueCount);
  m_QueueLocks.resize(m_queueCount);
  m_AddCounters.resize(m_queueCount);
  for (size_t i=0; i<m_queueCount; ++i)
  {
    m_QueueLocks[i] = new ASync_CriticalSection();// SpinLock();
    m_AddCounters[i] = 0;
  }
}

AOSContextQueueThreadPool_RoundRobinSwarm::~AOSContextQueueThreadPool_RoundRobinSwarm()
{
  try
  {
    for (size_t i=0; i<m_QueueLocks.size(); ++i)
      delete m_QueueLocks[i];
  }
  catch(...) {}
}

void AOSContextQueueThreadPool_RoundRobinSwarm::add(AOSContext *pContext)
{
  //a_Add context to the next queue
  volatile long currentQueue = ::InterlockedIncrement(&m_currentReadQueue) % m_queueCount;
  ALock lock(m_QueueLocks.at(currentQueue));

  ++m_AddCounters.at(currentQueue);
  m_Queues.at(currentQueue).push_back(pContext);

  pContext->setExecutionState(getClass()+"::add["+AString::fromInt(currentQueue)+"]="+AString::fromPointer(pContext));
}                                                                                                                                                                                                 

//static ASync_CriticalSection ZZZZ;

AOSContext *AOSContextQueueThreadPool_RoundRobinSwarm::_nextContext()
{
  volatile long currentQueue = ::InterlockedIncrement(&m_currentReadQueue) % m_queueCount;
  ALock lock(m_QueueLocks.at(currentQueue));

  if (!m_Queues.at(currentQueue).empty())
  {
    AOSContext *pContext = m_Queues.at(currentQueue).front();
    m_Queues.at(currentQueue).pop_front();

    pContext->setExecutionState(getClass()+"::_nextContext["+AString::fromInt(currentQueue)+"]="+AString::fromPointer(pContext));

    return pContext;
  }
  else
    return NULL;

  //a_Next queue
    //int currentQueue;
    //{
    //  ALock lock(ZZZZ);
    //  currentQueue = m_currentReadQueue;
    //  m_currentReadQueue = (m_currentReadQueue + 1) % m_queueCount;
    //}

    //AOSContext *pContext = NULL;

//  int tries = 0;
//  while (!pContext && tries < m_queueCount)
//  {
//    ALock lock(m_QueueLocks.at(m_currentReadQueue));
//    if (!m_Queues.at(m_currentReadQueue).empty())
//    {
//      pContext = m_Queues.at(m_currentReadQueue).front();
//      m_Queues.at(m_currentReadQueue).pop_front();
//
//      pContext->setExecutionState(getClass()+"::_nextContext["+AString::fromInt(m_currentReadQueue)+"]="+AString::fromPointer(pContext));
//    }
////    else
////      ++tries;
////  }
//
//  return pContext;
}
