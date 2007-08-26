#include "pchAOS_Base.hpp"
#include "AOSContextQueueThreadPool.hpp"
#include "ALock.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"

AOSContextQueueThreadPool::AOSContextQueueThreadPool(
  AOSServices& services,
  size_t threadCount,                 // = 1
  AOSContextQueueInterface *pForward, // = NULL, 
  AOSContextQueueInterface *pBack,    // = NULL
  AOSContextQueueInterface *pError    // = NULL
) :
  m_ThreadPool(AOSContextQueueThreadPool::_threadprocWrapper, threadCount),
  AOSContextQueueInterface(services, pForward, pBack, pError),
  m_SleepDelay(DEFAULT_SLEEP_DELAY)
{
  m_ThreadPool.setThis(this);
}

AOSContextQueueThreadPool::~AOSContextQueueThreadPool()
{
}

void AOSContextQueueThreadPool::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueInterface::addAdminXml(eBase, request);

  addProperty(
    eBase,
    ASW("ThreadPool.threadCount",22),
    AString::fromSize_t(m_ThreadPool.getThreadCount())
  );

  addPropertyWithAction(
    eBase, 
    ASW("ThreadPool.sleepDelay",21),
    AString::fromInt(m_SleepDelay),
    ASW("Update",6), 
    ASW("Sleep delay",11),
    ASW("newSleepDelay",13)
  );

  addPropertyWithAction(
    eBase, 
    ASW("ThreadPool.threadCount",22),
    AString::fromSize_t(m_ThreadPool.getThreadCount()),
    ASW("Update",6), 
    ASW("Thread count",12),
    ASW("newThreadCount",14)
  );
}

void AOSContextQueueThreadPool::processAdminAction(AXmlElement& base, const AHTTPRequestHeader& request)
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
  AOSContextQueueThreadPool *pThis = static_cast<AOSContextQueueThreadPool *>(thread.getThis());
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
