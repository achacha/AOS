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

void AOSContextQueueThreadPool_RoundRobinSwarm::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueThreadPool::adminEmitXml(eBase, request);

  adminAddProperty(
    eBase,
    ASW("CurrentReadQueue",16),
    AString::fromInt(m_currentReadQueue % m_queueCount)
  );

  adminAddProperty(
    eBase,
    ASW("CurrentWriteQueue",17),
    AString::fromInt(m_currentWriteQueue % m_queueCount)
  );

  adminAddProperty(
    eBase,
    ASW("Queues.size",11),
    AString::fromSize_t(m_Queues.size())
  );

  for (size_t i=0; i<m_AddCounters.size(); ++i)
  {
    adminAddProperty(
      eBase,
      ARope("Queue[")+AString::fromSize_t(i)+ASW("].count",7),
      AString::fromSize_t(m_AddCounters[i])
    );

    adminAddProperty(
      eBase,
      ARope("Queue[")+AString::fromSize_t(i)+ASW("].isEmpty",9),
      AString::fromSize_t(m_Queues[i]->isEmpty())
    );
  }
}

void AOSContextQueueThreadPool_RoundRobinSwarm::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueThreadPool::adminProcessAction(eBase, request);
}

AOSContextQueueThreadPool_RoundRobinSwarm::AOSContextQueueThreadPool_RoundRobinSwarm(
  AOSServices& services,
  size_t threadCount,              // = 16
  size_t queueCount                // = 4
) :
  m_queueCount(queueCount),
  AOSContextQueueThreadPool(services, threadCount),
  m_currentWriteQueue(0),
  m_currentReadQueue(queueCount-1)
{
  AASSERT(this, queueCount > 0);
  m_Queues.resize(m_queueCount);
  m_AddCounters.resize(m_queueCount);
  for (size_t i=0; i<m_queueCount; ++i)
  {
    m_Queues[i] = new ABasePtrQueue(new ASync_CriticalSection());
    m_AddCounters[i] = 0;
  }
}

AOSContextQueueThreadPool_RoundRobinSwarm::~AOSContextQueueThreadPool_RoundRobinSwarm()
{
  try
  {
    for (size_t i=0; i<m_Queues.size(); ++i)
    {
      m_Queues[i]->clear(true);
      delete m_Queues[i];
    }
  }
  catch(...) {}
}

void AOSContextQueueThreadPool_RoundRobinSwarm::add(AOSContext *pContext)
{
  //a_Add context to the next queue
  volatile long currentQueue = ::InterlockedIncrement(&m_currentReadQueue) % m_queueCount;
  ++m_AddCounters.at(currentQueue);
  m_Queues.at(currentQueue)->push(pContext);

//  std::cout << typeid(*this).name() << ":" << getClass() <<  "::add(" << AString::fromPointer(pContext) << "): " << pContext->useRequestParameterPairs() << " (" << pContext->useRequestUrl() << ")" << std::endl;

  pContext->setExecutionState(ARope(getClass())+"::add["+AString::fromInt(currentQueue)+"]="+AString::fromPointer(pContext));
}                                                                                                                                                                                                 

AOSContext *AOSContextQueueThreadPool_RoundRobinSwarm::_nextContext()
{
  volatile long currentQueue = ::InterlockedIncrement(&m_currentReadQueue) % m_queueCount;
  AOSContext *pContext = (AOSContext *)m_Queues.at(currentQueue)->pop();
  if (pContext)
  {
    //    std::cout << typeid(*this).name() << ":" << getClass() <<  "::next(" << AString::fromPointer(pContext) << "): " << pContext->useRequestParameterPairs() << " (" << pContext->useRequestUrl() << ")" << std::endl;

    pContext->setExecutionState(ARope(getClass())+"::_nextContext["+AString::fromInt(currentQueue)+"]="+AString::fromPointer(pContext));
  }
  return pContext;
}
