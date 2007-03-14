#include "pchAOS_Base.hpp"
#include "AOSRequestQueue_HttpReader.hpp"
#include "AOSRequest.hpp"
#include "AOSServices.hpp"
#include "AOSContext.hpp"
\
AOSRequestQueue_HttpReader::AOSRequestQueue_HttpReader(
  AOSServices& services,
  int threadCount,                    // = 1
  AOSRequestQueueInterface *pForward, // = NULL 
  AOSRequestQueueInterface *pBack     // = NULL
) :
  AOSRequestQueueThreadPool(services, threadCount, pForward, pBack)
{
}
AOSRequestQueue_HttpReader::~AOSRequestQueue_HttpReader()
{
}

u4 AOSRequestQueue_HttpReader::_threadproc(AThread& thread)
{
  AOSRequestQueue_HttpReader *pThis = static_cast<AOSRequestQueue_HttpReader *>(thread.getThis());
  AASSERT(NULL, pThis);

  AOSRequest *pRequest = NULL;
  AString str(2048, 256);

  thread.setRunning(true);
  while (thread.isRun())
  {
    if (pRequest = pThis->_nextRequest())
    {
      str.clear();
      size_t ret = pRequest->useSocket().read(str, 2048);
      pRequest->useBuffer().append(str);

      if (pRequest->useBuffer().find(ASW("\r\n\r\n", 4)))
      {
        //a_Advance to context based processing
        pThis->_goForward(pRequest);
      }
      else
      {
        //a_Header not complete
        pThis->_goBack(pRequest);
      }
    }
    AThread::sleep(10);
  }
  thread.setRunning(false);

  return 0;
}

void AOSRequestQueue_HttpReader::add(AOSRequest *pRequest)
{
  ALock lock(m_SynchObject);
  m_Queue.push_back(pRequest);

  static const AString LOG_MSG("AOSRequestQueue_HttpReader::add");
  m_AOSServices.useLog().add(LOG_MSG, AString::fromPointer(pRequest), ALog::DEBUG);
}


AOSRequest *AOSRequestQueue_HttpReader::_nextRequest()
{
  ALock lock(m_SynchObject);
  if (m_Queue.empty())
    return NULL;
  else
  {
    AOSRequest *pRequest = m_Queue.front();
    m_Queue.pop_front();

    static const AString LOG_MSG("AOSRequestQueue_HttpReader::_nextRequest");
    m_AOSServices.useLog().add(LOG_MSG, AString::fromPointer(pRequest), ALog::DEBUG);

    return pRequest;
  }
}

/*
void AOSRequestQueue_HttpReader::_goForward(AOSRequest *pRequest)
{
  if (mp_Forward)
    mp_Forward->add(pRequest);
  else
    AOSRequest::deallocate(pRequest);
}

void AOSRequestQueue_HttpReader::_goBack(AOSRequest *pRequest)
{
  if (mp_Back)
    mp_Back->add(pRequest);
  else
    AOSRequest::deallocate(pRequest);
}
*/
