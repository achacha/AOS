#include "pchAOS_Base.hpp"
#include "AOSContextQueue_IsAvailable.hpp"
#include "AOSContext.hpp"
#include "AFile_Socket.hpp"
#include "AOSServices.hpp"
#include "AThread.hpp"

AOSContextQueue_IsAvailable::AOSContextQueue_IsAvailable(
  AOSServices& services,
  size_t queueCount  // = 1
) :
  AOSContextQueueInterface(services),
  m_AddCounter(0)
{
  m_Queues.resize(queueCount);

  adminRegisterObject(services.useAdminRegistry());
}

AOSContextQueue_IsAvailable::~AOSContextQueue_IsAvailable()
{
  try
  {
    stopQueues();
  }
  catch(...) {}
}

const AString& AOSContextQueue_IsAvailable::getClass() const
{
  static const AString CLASS("AOSContextQueue_IsAvailable");
  return CLASS;
}

void AOSContextQueue_IsAvailable::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueInterface::adminEmitXml(eBase, request);

  adminAddProperty(
    eBase,
    ASW("Queues.size",11),
    AString::fromSize_t(m_Queues.size())
  );
}

void AOSContextQueue_IsAvailable::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueInterface::adminProcessAction(eBase, request);
}


AOSContextQueue_IsAvailable::QueueWithThread::QueueWithThread() :
  pthread(NULL) 
{
}

AOSContextQueue_IsAvailable::QueueWithThread::~QueueWithThread()
{
}

void AOSContextQueue_IsAvailable::QueueWithThread::startThread()
{
  AASSERT(NULL, NULL == pthread);
  pthread = new AThread(_threadprocWorker, true, this);
}

void AOSContextQueue_IsAvailable::startQueues()
{
  for (size_t i=0; i<m_Queues.size(); ++i)
  {
    m_Queues.at(i).startThread();
  }
}

void AOSContextQueue_IsAvailable::stopQueues()
{
  for (size_t i=0; i<m_Queues.size(); ++i)
  {
    AASSERT(this, m_Queues.at(i).pthread);
    m_Queues.at(i).pthread->setRun(false);
  }
  
  int tries = 3;
  bool stillStopping = true;
  while (stillStopping)
  {
    size_t i;
    for (i=0; i<m_Queues.size(); ++i)
    {
      if (m_Queues.at(i).pthread->isRunning())
      {
        AThread::sleep(200);
        --tries;
        break;
      }
      else
      {
        pDelete(m_Queues.at(i).pthread);
      }
    }
    if (i == m_Queues.size())
      return;

    if (tries < 0)
      break;
  }

  //a_Terminate hung threads
  for (size_t i=0; i<m_Queues.size(); ++i)
  {
    if (m_Queues.at(i).pthread && m_Queues.at(i).pthread->isRunning())
    {
      m_Queues.at(i).pthread->terminate();
      delete m_Queues.at(i).pthread;
    }
  }
}

u4 AOSContextQueue_IsAvailable::_threadprocWorker(AThread& thread)
{
  AOSContextQueue_IsAvailable::QueueWithThread *pThis = dynamic_cast<AOSContextQueue_IsAvailable::QueueWithThread *>(thread.getThis());
  AASSERT(&thread, pThis);

  timeval timeout;
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  static const int NO_DATA_TIMEOUT = m_Services.useConfiguration().getConfigRoot().getInt(ASW("/config/server/context-queue/is-available/no-data-timeout",57), 30000);

  thread.setRunning(true);
  while (thread.isRun())
  {
    try
    {
      if (pThis->queue.size() > 0)
      {
        fd_set sockSet;
        FD_ZERO(&sockSet);
        
        int count = 0;
        REQUESTS::iterator it = pThis->queue.begin();
        while (count < FD_SETSIZE && it != pThis->queue.end())
        {
          FD_SET((*it)->useSocket().getSocketInfo().m_handle, &sockSet);
          ++count;
          ++it;
        }
        
        if (count > 0)
        {
          int availCount = ::select(count, &sockSet, NULL, NULL, &timeout);
          if (availCount > 0)
          {
            int count2 = 0;
            it = pThis->queue.begin();
            
            while (availCount > 0 && count2 <= count && it != m_Queue.end())
            {
              if (FD_ISSET((*it)->useSocket().getSocketInfo().m_handle, &sockSet))
              {
                REQUESTS::iterator itMove = it;
                ++it;

                (*itMove)->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_AVAILABLE_SELECTED);
                (*itMove)->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_AVAILABLE_PENDING);

                {
                  //a_Add to next queue
                  ALock lock(pThis->sync);
                  (*itMove)->setExecutionState(ASW("AOSContextQueue_IsAvailable: HTTP header has more data",54));
                  m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_PRE_EXECUTE, &(*itMove));
                  pThis->queue.erase(itMove);
                }

                --availCount;
              }
              else
              {
                //a_Select called on this AOSContext and no data available
                if ((*it)->useTimeoutTimer().getInterval() > NO_DATA_TIMEOUT)
                {
                  REQUESTS::iterator itMove = it;
                  ++it;

                  AString str;
                  str.append("AOSContextQueue_IsAvailable: No data after timeout: ",52);
                  (*itMove)->useTimeoutTimer().emit(str);

                  //a_We are done with this request, still no data
                  ALock lock(pThis->sync);
                  (*itMove)->setExecutionState(
                    str, 
                    !(*itMove)->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING)
                  );
                  m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &(*itMove));
                  pThis->queue.erase(itMove);
                }
                else
                {
                  (*it)->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_AVAILABLE_SELECTED);
                  ++it;
                }
              }
              ++count2;
            }
            AASSERT(pThis, !availCount);
          }
          else
          {
            //a_Nothing has data, flag them all
            it = pThis->queue.begin();
            while (it != pThis->queue.end())
            {
              if ((*it)->useTimeoutTimer().getInterval() > NO_DATA_TIMEOUT)
              {
                REQUESTS::iterator itMove = it;
                ++it;

                AString str("AOSContextQueue_IsAvailable: No data after timeout: ",52);
                (*itMove)->useTimeoutTimer().emit(str);

                //a_We are done with this request, still no data
                ALock lock(pThis->sync);
                (*itMove)->setExecutionState(
                  str, 
                  !(*itMove)->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING)
                );
                m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &(*itMove));
                pThis->queue.erase(itMove);
              }
              else
              {
                (*it)->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_AVAILABLE_SELECTED);
                ++it;
              }
            }
          }
        }
        
        //a_Sleep between selects, requests in this queue are already slow
        AThread::sleep(10);
      }
      else
        AThread::sleep(30);    //a_Nothing in queue, deep sleep
    }
    catch(AException& e)
    {
      pThis->m_Services.useLog().addException(e);
    }
    catch(std::exception& e)
    {
      pThis->m_Services.useLog().addException(e);
    }
    catch(...)
    {
      pThis->m_Services.useLog().add(ASWNL("Unknown exception caught in AOSContextQueue_IsAvailable::threadproc"), ALog::EXCEPTION);
    }
  }

  thread.setRunning(false);
  return 0;
}

void AOSContextQueue_IsAvailable::add(AOSContext *pContext)
{
  //a_Clear these in case this is not the first trip here
  //a_More than one trip can result if some bytes were read but not enough
  pContext->useConnectionFlags().clearBit(AOSContext::CONFLAG_IS_AVAILABLE_SELECTED);
  pContext->useConnectionFlags().clearBit(AOSContext::CONFLAG_IS_AVAILABLE_PENDING);

  static const AString LOG_MSG("AOSContextQueue_IsAvailable::add");
  pContext->setExecutionState(LOG_MSG);

  {
    ALock lock(m_SynchObjectContextContainer);
    ++m_AddCounter;

    //a_Start timeout timer
    pContext->useTimeoutTimer().start();
    m_Queues.at(m_AddCounter % m_Queues.size()).push_back(pContext);
  }
}

AOSContext *AOSContextQueue_IsAvailable::_nextContext()
{
  //a_The way the queues work, this method is never used, each queue owns their own set of contexts for select
  ATHROW(this, AException::ProgrammingError);
}
