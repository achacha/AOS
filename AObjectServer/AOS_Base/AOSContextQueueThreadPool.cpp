#include "pchAOS_Base.hpp"
#include "AOSContextQueueThreadPool.hpp"
#include "ALock.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"

AOSContextQueueThreadPool::AOSContextQueueThreadPool(
  AOSServices& services,
  int threadCount,                    // = 1
  AOSContextQueueInterface *pForward, // = NULL, 
  AOSContextQueueInterface *pBack,    // = NULL
  AOSContextQueueInterface *pError    // = NULL
) :
  m_ThreadPool(AOSContextQueueThreadPool::_threadprocWrapper, threadCount),
  AOSContextQueueInterface(services, pForward, pBack, pError)
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
}

u4 AOSContextQueueThreadPool::_threadprocWrapper(AThread& thread)
{
  AOSContextQueueThreadPool *pThis = static_cast<AOSContextQueueThreadPool *>(thread.getThis());
  AASSERT(pThis, pThis);
  AASSERT(pThis, ADebugDumpable::isPointerValid(pThis));

  return pThis->_threadproc(thread);
}

