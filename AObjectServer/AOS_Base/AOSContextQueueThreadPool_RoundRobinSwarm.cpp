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
    AString::fromInt(m_currentReadQueue)
  );

  addProperty(
    eBase,
    ASW("CurrentWriteQueue",17),
    AString::fromInt(m_currentWriteQueue)
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
  int threadCount,                 // = 16
  int queueCount,                  // = 4
  AOSContextQueueInterface *pYes,  // = NULL 
  AOSContextQueueInterface *pNo,   // = NULL
  AOSContextQueueInterface *pError // = NULL
) :
  m_queueCount(queueCount),
  AOSContextQueueThreadPool(services, threadCount, pYes, pNo, pError),
  m_currentWriteQueue(0),
  m_currentReadQueue(0)
{
  m_Queues.resize(m_queueCount);
  m_QueueLocks.resize(m_queueCount);
  m_AddCounters.resize(m_queueCount);
  for (int i=0; i<m_queueCount; ++i)
  {
    m_QueueLocks[i] = new ACriticalSectionSpinLock();
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
  ALock lock(m_QueueLocks[m_currentWriteQueue]);
  ++m_AddCounters[m_currentWriteQueue];
  m_Queues[m_currentWriteQueue].push_back(pContext);
  m_currentWriteQueue = (m_currentWriteQueue + 1) % m_queueCount;

  pContext->setExecutionState(getClass()+"::add"+AString::fromPointer(pContext));
}

AOSContext *AOSContextQueueThreadPool_RoundRobinSwarm::_nextContext()
{
  ALock lock(m_QueueLocks[m_currentReadQueue]);
  AOSContext *pContext = NULL;
  
  int tries = 0;
  while (!pContext && tries < m_queueCount)
  {
    if (!m_Queues[m_currentReadQueue].empty())
    {
      pContext = m_Queues[m_currentReadQueue].front();
      m_Queues[m_currentReadQueue].pop_front();
      m_currentReadQueue = (m_currentReadQueue + 1) % m_queueCount;

      pContext->setExecutionState(getClass()+"::_nextContext"+AString::fromPointer(pContext));

      break;
    }
    m_currentReadQueue = (m_currentReadQueue + 1) % m_queueCount;
    ++tries;
  }
  return pContext;
}
