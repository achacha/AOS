/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSContextQueueThreadPool_RoundRobinSwarm.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include <typeinfo>

//a_Delay delay for loop sleep in milliseconds
const int AOSContextQueueThreadPool_RoundRobinSwarm::DEFAULT_SLEEP_DELAY(5);

const AString AOSContextQueueThreadPool_RoundRobinSwarm::CLASS("AOSContextQueueThreadPool_RoundRobinSwarm");

const AString& AOSContextQueueThreadPool_RoundRobinSwarm::getClass() const
{
  return CLASS;
}

void AOSContextQueueThreadPool_RoundRobinSwarm::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueInterface::adminEmitXml(eBase, request);

  adminAddProperty(
    eBase,
    ASW("ThreadPool.threadCount",22),
    AString::fromSize_t(m_ThreadPool.getThreadCount())
  );

  adminAddPropertyWithAction(
    eBase, 
    ASW("ThreadPool.sleepDelay",21),
    AString::fromInt(m_SleepDelay),
    ASW("Update",6), 
    ASW("Sleep delay",11),
    ASW("newSleepDelay",13)
  );

  adminAddPropertyWithAction(
    eBase, 
    ASW("ThreadPool.threadCount",22),
    AString::fromSize_t(m_ThreadPool.getThreadCount()),
    ASW("Update",6), 
    ASW("Thread count",12),
    ASW("newThreadCount",14)
  );

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
      ARope("Queue[")+AString::fromSize_t(i)+ASW("].size",6),
      AString::fromSize_t(m_Queues[i]->size())
    );
  }
}

void AOSContextQueueThreadPool_RoundRobinSwarm::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueInterface::adminProcessAction(eBase, request);
}

AOSContextQueueThreadPool_RoundRobinSwarm::AOSContextQueueThreadPool_RoundRobinSwarm(
  AOSServices& services,
  size_t threadCount,              // = 16
  size_t queueCount                // = 4
) :
  m_ThreadPool(AOSContextQueueThreadPool_RoundRobinSwarm::_threadprocWrapperRoundRobbin, threadCount),
  AOSContextQueueInterface(services),
  m_SleepDelay(DEFAULT_SLEEP_DELAY),
  m_queueCount(queueCount),
  m_currentWriteQueue(0),
  m_currentReadQueue(((long)queueCount)-1)
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

u4 AOSContextQueueThreadPool_RoundRobinSwarm::_threadprocWrapperRoundRobbin(AThread& thread)
{
  AASSERT(NULL, thread.getThis());
  AASSERT(NULL, ADebugDumpable::isPointerValid(thread.getThis()));
  AOSContextQueueThreadPool_RoundRobinSwarm *pThis = dynamic_cast<AOSContextQueueThreadPool_RoundRobinSwarm *>(thread.getThis());
  AASSERT(pThis, pThis);
  AASSERT(pThis, ADebugDumpable::isPointerValid(pThis));

  return pThis->_threadproc(thread);
}

AOSContextQueueThreadPool_RoundRobinSwarm::~AOSContextQueueThreadPool_RoundRobinSwarm()
{
  try
  {
    for (size_t i=0; i<m_Queues.size(); ++i)
    {
      m_Queues[i]->clear(true);
      delete m_Queues.at(i);
    }
  }
  catch(...) {}
}

void AOSContextQueueThreadPool_RoundRobinSwarm::add(AOSContext *pContext)
{
  //a_Add context to the next queue
  volatile long currentQueue = ::InterlockedIncrement(&m_currentReadQueue) % m_queueCount;
  if (pContext && pContext->useEventVisitor().isLoggingDebug())
  {
    AString str;
    str.append(getClass());
    str.append("::add[",6);
    str.append(AString::fromInt(currentQueue));
    str.append("]=",2);
    str.append(AString::fromPointer(pContext));
    pContext->useEventVisitor().startEvent(str, AEventVisitor::EL_DEBUG);
  }

  ++m_AddCounters.at(currentQueue);
  m_Queues.at(currentQueue)->pushBack(pContext);
}                                                                                                                                                                                                 

AOSContext *AOSContextQueueThreadPool_RoundRobinSwarm::_nextContext()
{
  volatile long currentQueue = ::InterlockedIncrement(&m_currentReadQueue) % m_queueCount;
  AOSContext *pContext = (AOSContext *)m_Queues.at(currentQueue)->popFront();

  if (pContext && pContext->useEventVisitor().isLoggingDebug())
  {
    AString str;
    str.append(getClass());
    str.append("::_nextContext[",15);
    str.append(AString::fromInt(currentQueue));
    str.append("]=",2);
    str.append(AString::fromPointer(pContext));
    pContext->useEventVisitor().startEvent(str, AEventVisitor::EL_DEBUG);
  }

  return pContext;
}

void AOSContextQueueThreadPool_RoundRobinSwarm::setSleepDelay(int sleepDelay)
{
  m_SleepDelay = sleepDelay;
}

int AOSContextQueueThreadPool_RoundRobinSwarm::getSleepDelay() const
{
  return m_SleepDelay;
}

bool AOSContextQueueThreadPool_RoundRobinSwarm::isRunning()
{
  return m_ThreadPool.isRunning();
}
