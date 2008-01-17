#include "pchAOS_Base.hpp"
#include "AOSContextQueue_IsAvailable_SingleQueue.hpp"
#include "AOSContext.hpp"
#include "AFile_Socket.hpp"
#include "AOSServices.hpp"

AOSContextQueue_IsAvailable_SingleQueue::AOSContextQueue_IsAvailable_SingleQueue(
  AOSServices& services
) :
  AOSContextQueueThreadPool(services, 1),
  m_AddCounter(0)
{
  adminRegisterObject(services.useAdminRegistry());
}

AOSContextQueue_IsAvailable_SingleQueue::~AOSContextQueue_IsAvailable_SingleQueue()
{
}

const AString& AOSContextQueue_IsAvailable_SingleQueue::getClass() const
{
  static const AString CLASS("AOSContextQueue_IsAvailable_SingleQueue");
  return CLASS;
}

void AOSContextQueue_IsAvailable_SingleQueue::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueThreadPool::adminEmitXml(eBase, request);

  adminAddProperty(
    eBase,
    ASW("Queue.size",10),
    AString::fromSize_t(m_Queue.size())
  );
}

void AOSContextQueue_IsAvailable_SingleQueue::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueThreadPool::adminProcessAction(eBase, request);
}

u4 AOSContextQueue_IsAvailable_SingleQueue::_threadproc(AThread& thread)
{
  AOSContextQueue_IsAvailable_SingleQueue *pThis = dynamic_cast<AOSContextQueue_IsAvailable_SingleQueue *>(thread.getThis());
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
      if (m_Queue.size() > 0)
      {
        fd_set sockSet;
        FD_ZERO(&sockSet);
        
        int count = 0;
        REQUESTS::iterator it = m_Queue.begin();
        while (count < FD_SETSIZE && it != m_Queue.end())
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
            it = m_Queue.begin();
            
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
                  ALock lock(m_SynchObjectContextContainer);
                  (*itMove)->setExecutionState(ASW("AOSContextQueue_IsAvailable_SingleQueue: HTTP header has more data",54));
                  m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_PRE_EXECUTE, &(*itMove));
                  m_Queue.erase(itMove);
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
                  str.append("AOSContextQueue_IsAvailable_SingleQueue: No data after timeout: ",52);
                  (*itMove)->useTimeoutTimer().emit(str);

                  //a_We are done with this request, still no data
                  ALock lock(m_SynchObjectContextContainer);
                  (*itMove)->setExecutionState(
                    str, 
                    !(*itMove)->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING)
                  );
                  m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &(*itMove));
                  m_Queue.erase(itMove);
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
            it = m_Queue.begin();
            while (it != m_Queue.end())
            {
              if ((*it)->useTimeoutTimer().getInterval() > NO_DATA_TIMEOUT)
              {
                REQUESTS::iterator itMove = it;
                ++it;

                AString str("AOSContextQueue_IsAvailable_SingleQueue: No data after timeout: ",52);
                (*itMove)->useTimeoutTimer().emit(str);

                //a_We are done with this request, still no data
                ALock lock(m_SynchObjectContextContainer);
                (*itMove)->setExecutionState(
                  str, 
                  !(*itMove)->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING)
                );
                m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &(*itMove));
                m_Queue.erase(itMove);
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
      pThis->m_Services.useLog().add(ASWNL("Unknown exception caught in AOSContextQueue_IsAvailable_SingleQueue::threadproc"), ALog::EXCEPTION);
    }
  }

  thread.setRunning(false);
  return 0;
}

void AOSContextQueue_IsAvailable_SingleQueue::add(AOSContext *pContext)
{
  ALock lock(m_SynchObjectContextContainer);
  ++m_AddCounter;

  //a_Clear these in case this is not the first trip here
  //a_More than one trip can result if some bytes were read but not enough
  pContext->useConnectionFlags().clearBit(AOSContext::CONFLAG_IS_AVAILABLE_SELECTED);
  pContext->useConnectionFlags().clearBit(AOSContext::CONFLAG_IS_AVAILABLE_PENDING);

  static const AString LOG_MSG("AOSContextQueue_IsAvailable_SingleQueue::add");
  pContext->setExecutionState(LOG_MSG);

  //a_Start timeout timer
  pContext->useTimeoutTimer().start();
  m_Queue.push_back(pContext);
}

AOSContext *AOSContextQueue_IsAvailable_SingleQueue::_nextContext()
{
  ALock lock(m_SynchObjectContextContainer);
  if (m_Queue.empty())
    return NULL;
  else
  {
    AOSContext *pContext = m_Queue.front();
    m_Queue.pop_front();

    static const AString LOG_MSG("AOSContextQueue_IsAvailable_SingleQueue::_nextRequest");
    pContext->setExecutionState(LOG_MSG);

    return pContext;
  }
}
