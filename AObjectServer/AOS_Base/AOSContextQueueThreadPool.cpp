/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSContextQueueThreadPool.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"

AOSContextQueueThreadPool::AOSContextQueueThreadPool(
  AOSServices& services,
  size_t threadCount                 // = 1
) :
  m_ThreadPool(AOSContextQueueThreadPool::_threadprocWrapper, threadCount),
  AOSContextQueueInterface(services),
  m_SleepDelay(DEFAULT_SLEEP_DELAY)
{
  m_ThreadPool.setThis(this);
}

AOSContextQueueThreadPool::~AOSContextQueueThreadPool()
{
}

void AOSContextQueueThreadPool::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
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
}

void AOSContextQueueThreadPool::adminProcessAction(AXmlElement& base, const AHTTPRequestHeader& request)
{
  AString str;
  if (request.getUrl().getParameterPairs().get(ASW("newThreadCount",14), str))
  {
    int newCount = str.toInt();
    if (newCount > 1)
      m_ThreadPool.setThreadCount((size_t)newCount);
  }
  else if (request.getUrl().getParameterPairs().get(ASW("newSleepDelay",13), str))
  {
    int newDelay = str.toInt();
    if (newDelay > 0)
      m_SleepDelay = newDelay;
  }
}

u4 AOSContextQueueThreadPool::_threadprocWrapper(AThread& thread)
{
  AOSContextQueueThreadPool *pThis = dynamic_cast<AOSContextQueueThreadPool *>(thread.getThis());
  AASSERT(pThis, pThis);
  AASSERT(pThis, ADebugDumpable::isPointerValid(pThis));

  return pThis->_threadproc(thread);
}

void AOSContextQueueThreadPool::setSleepDelay(int sleepDelay)
{
  m_SleepDelay = sleepDelay;
}

int AOSContextQueueThreadPool::getSleepDelay() const
{
  return m_SleepDelay;
}
