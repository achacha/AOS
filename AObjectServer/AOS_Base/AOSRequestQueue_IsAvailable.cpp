#include "pchAOS_Base.hpp"
#include "AOSRequestQueue_IsAvailable.hpp"
#include "AOSRequest.hpp"
#include "AFile_Socket.hpp"
#include "AOSServices.hpp"

AOSRequestQueue_IsAvailable::AOSRequestQueue_IsAvailable(
  AOSServices& services,
  AOSRequestQueueInterface *pForward, // = NULL 
  AOSRequestQueueInterface *pBack     // = NULL
) :
  AOSRequestQueueThreadPool(services, 1, pForward, pBack)
{
}
AOSRequestQueue_IsAvailable::~AOSRequestQueue_IsAvailable()
{
}

u4 AOSRequestQueue_IsAvailable::_threadproc(AThread& thread)
{
  AOSRequestQueue_IsAvailable *pThis = static_cast<AOSRequestQueue_IsAvailable *>(thread.getThis());
  AASSERT(NULL, pThis);

  timeval timeout;
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  thread.setRunning(true);
  while (thread.isRun())
  {
    if (m_Queue.size() > 0)
    {
      fd_set sockSet;
      FD_ZERO(&sockSet);
      
      int count = 0;
      REQUESTS::iterator it = m_Queue.begin();
      while (count < FD_SETSIZE && it != m_Queue.end())
      {
        FD_SET((*it)->useSocket().getSocketHandle(), &sockSet);
        ++count;
        ++it;
      }
      
      if (count > 0)
      {
        int availCount = ::select(count, &sockSet, NULL, NULL, &timeout);

        int count2 = 0;
        it = m_Queue.begin();
        while (availCount > 0 && count2 <= count && it != m_Queue.end())
        {
          if (FD_ISSET((*it)->useSocket().getSocketHandle(), &sockSet))
          {
            REQUESTS::iterator itMove = it;
            ++it;

            {
              ALock lock(m_SynchObject);
              pThis->_goForward(*itMove);
              m_Queue.erase(itMove);
            }

            --availCount;
          }
          else
            ++it;

          ++count2;
        }
        AASSERT(pThis, !availCount);
      }
    }
    else
      AThread::sleep(10);
  }
  thread.setRunning(false);

  return 0;
}

void AOSRequestQueue_IsAvailable::add(AOSRequest *pRequest)
{
  ALock lock(m_SynchObject);
  m_Queue.push_back(pRequest);

  static const AString LOG_MSG("AOSRequestQueue_IsAvailable::add");
  m_AOSServices.useLog().add(LOG_MSG, AString::fromPointer(pRequest), ALog::DEBUG);
}

AOSRequest *AOSRequestQueue_IsAvailable::_nextRequest()
{
  ALock lock(m_SynchObject);
  if (m_Queue.empty())
    return NULL;
  else
  {
    AOSRequest *pRequest = m_Queue.front();
    m_Queue.pop_front();

    static const AString LOG_MSG("AOSRequestQueue_IsAvailable::_nextRequest");
    m_AOSServices.useLog().add(LOG_MSG, AString::fromPointer(pRequest), ALog::DEBUG);

    return pRequest;
  }
}

/*
void AOSRequestQueue_IsAvailable::_goForward(AOSRequest *pRequest)
{
  if (mp_Forward)
    mp_Forward->add(pRequest);
  else
    AOSRequest::deallocate(pRequest);
}

void AOSRequestQueue_IsAvailable::_goBack(AOSRequest *pRequest)
{
  if (mp_Back)
    mp_Back->add(pRequest);
  else
    AOSRequest::deallocate(pRequest);
}
*/