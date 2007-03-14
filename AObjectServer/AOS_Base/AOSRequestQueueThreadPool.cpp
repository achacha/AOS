#include "pchAOS_Base.hpp"
#include "AOSRequestQueueThreadPool.hpp"
#include "ALock.hpp"
#include "AOSRequest.hpp"

AOSRequestQueueThreadPool::AOSRequestQueueThreadPool(
  AOSServices& services,
  int threadCount,                    // = 1
  AOSRequestQueueInterface *pForward, // = NULL, 
  AOSRequestQueueInterface *pBack     // = NULL
) :
  AThreadPool(AOSRequestQueueThreadPool::_threadprocWrapper, threadCount),
  AOSRequestQueueInterface(services, pForward, pBack)
{
}

AOSRequestQueueThreadPool::~AOSRequestQueueThreadPool()
{
}

u4 AOSRequestQueueThreadPool::_threadprocWrapper(AThread& thread)
{
  AOSRequestQueueThreadPool *pThis = static_cast<AOSRequestQueueThreadPool *>(thread.getThis());
  AASSERT(NULL, pThis);

  return pThis->_threadproc(thread);
}

