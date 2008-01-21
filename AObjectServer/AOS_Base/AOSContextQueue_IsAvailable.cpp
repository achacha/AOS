#include "pchAOS_Base.hpp"
#include "AOSContextQueue_IsAvailable.hpp"
#include "AOSContext.hpp"
#include "AFile_Socket.hpp"
#include "AOSServices.hpp"
#include "AThread.hpp"

void AOSContextQueue_IsAvailable::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AOSContextQueue_IsAvailable @ " << std::hex << this << std::dec << ") {" << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Queues={" << std::endl;
  {
    ALock lock(const_cast<ASync_CriticalSectionSpinLock *>(&m_SynchObjectContextContainer));
    for (size_t i=0; i<m_Queues.size(); ++i)
    {
      const QueueWithThread *pq = m_Queues.at(i);
      ADebugDumpable::indent(os, indent+2) << "queue[" << i << "].pthread=" << AString::fromPointer(pq->pthread) << std::endl;
      ADebugDumpable::indent(os, indent+2) << "queue[" << i << "].count=" << pq->count << std::endl;
      ADebugDumpable::indent(os, indent+2) << "queue[" << i << "].queue.size=" << pq->queue.size() << std::endl;
    }
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AOSContextQueue_IsAvailable::AOSContextQueue_IsAvailable(
  AOSServices& services,
  size_t queueCount  // = 1
) :
  AOSContextQueueInterface(services),
  m_queueCount(queueCount),
  m_currentQueue(0)
{
  m_Queues.resize(m_queueCount);
  for (size_t i=0; i<m_queueCount; ++i)
    m_Queues.at(i) = new AOSContextQueue_IsAvailable::QueueWithThread();

  adminRegisterObject(services.useAdminRegistry());
}

AOSContextQueue_IsAvailable::~AOSContextQueue_IsAvailable()
{
  try
  {
    stop();
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
    ASW("CurrentQueue",12),
    AString::fromInt(m_currentQueue % m_queueCount)
  );

  adminAddProperty(
    eBase,
    ASW("Queues.size",11),
    AString::fromSize_t(m_queueCount)
  );

  for (size_t i=0; i<m_Queues.size(); ++i)
  {
    QueueWithThread *p = m_Queues.at(i);
    AString str("queue[",6);
    str.append(AString::fromSize_t(i));

    adminAddProperty(
      eBase,
      str+ASW("].count",7),
      AString::fromSize_t(p->count)
    );

    adminAddProperty(
      eBase,
      str+ASW("].size",6),
      AString::fromSize_t(p->queue.size())
    );
  }
}

void AOSContextQueue_IsAvailable::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueInterface::adminProcessAction(eBase, request);
}


AOSContextQueue_IsAvailable::QueueWithThread::QueueWithThread() :
  pthread(NULL),
  count(0)
{
}

AOSContextQueue_IsAvailable::QueueWithThread::~QueueWithThread()
{
}

void AOSContextQueue_IsAvailable::start()
{
  for (size_t i=0; i<m_queueCount; ++i)
  {
    AASSERT(NULL, NULL == m_Queues.at(i)->pthread);
    m_Queues.at(i)->pthread = new AThread(_threadprocWorker, true, m_Queues.at(i), this);
  }
}

void AOSContextQueue_IsAvailable::stop()
{
  for (size_t i=0; i<m_Queues.size(); ++i)
  {
    AASSERT(this, m_Queues.at(i)->pthread);
    m_Queues.at(i)->pthread->setRun(false);
  }
  
  int tries = 3;
  bool stillStopping = true;
  while (stillStopping)
  {
    size_t i;
    for (i=0; i<m_Queues.size(); ++i)
    {
      QueueWithThread *pqt = m_Queues.at(i);
      if (pqt->pthread)
      {
        if (pqt->pthread->isRunning())
        {
          AThread::sleep(200);
          --tries;
          break;
        }
        else
        {
          pDelete(pqt->pthread);
        }
      }
    }
    if (i == m_Queues.size())
    {
      m_Queues.clear();
      return;
    }

    if (tries == 0)
      break;
  }

  //a_Terminate hung threads
  for (size_t i=0; i<m_Queues.size(); ++i)
  {
    if (m_Queues.at(i)->pthread && m_Queues.at(i)->pthread->isRunning())
    {
      m_Queues.at(i)->pthread->terminate();
      delete m_Queues.at(i)->pthread;
    }
  }
}

u4 AOSContextQueue_IsAvailable::_threadprocWorker(AThread& thread)
{
  AOSContextQueue_IsAvailable::QueueWithThread *pThis = dynamic_cast<AOSContextQueue_IsAvailable::QueueWithThread *>(thread.getThis());
  AOSContextQueue_IsAvailable *pOwner = dynamic_cast<AOSContextQueue_IsAvailable *>(thread.getParameter());
  AASSERT(&thread, pThis);
  AASSERT(&thread, pOwner);

  timeval timeout;
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  static const int NO_DATA_TIMEOUT = pOwner->m_Services.useConfiguration().getConfigRoot().getInt(ASW("/config/server/context-queue/is-available/no-data-timeout",57), 60000);
  static const int SLEEP_DELAY = pOwner->m_Services.useConfiguration().getConfigRoot().getInt(ASW("/config/server/context-queue/is-available/empty-queue-sleep-delay",65), 20);
  static const int LOOP_DELAY = pOwner->m_Services.useConfiguration().getConfigRoot().getInt(ASW("/config/server/context-queue/is-available/loop-delay",52), 5);

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
            
            while (availCount > 0 && count2 <= count && it != pThis->queue.end())
            {
              if (FD_ISSET((*it)->useSocket().getSocketInfo().m_handle, &sockSet))
              {
                REQUESTS::iterator itMove = it;
                ++it;

                (*itMove)->useConnectionFlags().setBit(AOSContext::CONFLAG_ISAVAILABLE_SELECT_SET);

                //a_Attempt to read some data to check if it actually has data, if not then connection is closed
                switch((*itMove)->useSocket().readBlockIntoLookahead())
                {
                  case AConstant::npos:
                  case 0:
                  {
                    //a_No data to read, socket is closed
                    (*itMove)->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_CLOSED);
                    (*itMove)->setExecutionState(ASW("AOSContextQueue_IsAvailable: Detected closed remote socket",58));
                    (*itMove)->useSocket().close();

                    {
                      ALock lock(pThis->sync);
                      pOwner->m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &(*itMove));
                      pThis->queue.erase(itMove);
                    }
                  }
                  break;

                  case AConstant::unavail:
                    if ((*itMove)->useTimeoutTimer().getInterval() > NO_DATA_TIMEOUT)
                    {
                      AString str;
                      str.append("AOSContextQueue_IsAvailable: No data (non-blocking) after timeout: ",67);
                      (*itMove)->useTimeoutTimer().emit(str);

                      //a_We are done with this request, still no data
                      (*itMove)->setExecutionState(
                        str, 
                        !(*itMove)->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING)
                      );
                      {
                        ALock lock(pThis->sync);
                        pOwner->m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &(*itMove));
                        pThis->queue.erase(itMove);
                      }
                    }
                  break;  // Would block, keep waiting

                  default:
                  {
                    //a_Add to next queue
                    (*itMove)->useConnectionFlags().setBit(AOSContext::CONFLAG_ISAVAILABLE_PENDING);
                    (*itMove)->setExecutionState(ASW("AOSContextQueue_IsAvailable: HTTP header has more data",54));

                    {
                      ALock lock(pThis->sync);
                      pOwner->m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_PRE_EXECUTE, &(*itMove));
                      pThis->queue.erase(itMove);
                    }
                  }
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
                  (*itMove)->setExecutionState(
                    str, 
                    !(*itMove)->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING)
                  );
                  {
                    ALock lock(pThis->sync);
                    pOwner->m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &(*itMove));
                    pThis->queue.erase(itMove);
                  }
                }
                else
                {
                  ++it;
                }
              }
              ++count2;
            }
            AASSERT(pOwner, !availCount);
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
                pOwner->m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &(*itMove));
                pThis->queue.erase(itMove);
              }
              else
              {
                ++it;
              }
            }
          }
        }
        
        //a_Sleep between selects, requests in this queue are already slow
        AThread::sleep(LOOP_DELAY);
      }
      else
        AThread::sleep(SLEEP_DELAY);    //a_Nothing in queue, deep sleep
    }
    catch(AException& e)
    {
      pOwner->m_Services.useLog().addException(e);
    }
    catch(std::exception& e)
    {
      pOwner->m_Services.useLog().addException(e);
    }
    catch(...)
    {
      pOwner->m_Services.useLog().add(ASWNL("Unknown exception caught in AOSContextQueue_IsAvailable::threadproc"), ALog::EXCEPTION);
    }
  }

  thread.setRunning(false);
  return 0;
}

void AOSContextQueue_IsAvailable::add(AOSContext *pContext)
{
  //a_Clear these in case this is not the first trip here
  //a_More than one trip can result if some bytes were read but not enough
  pContext->useConnectionFlags().clearBit(AOSContext::CONFLAG_ISAVAILABLE_SELECT_SET);
  pContext->useConnectionFlags().clearBit(AOSContext::CONFLAG_ISAVAILABLE_PENDING);

  static const AString LOG_MSG("AOSContextQueue_IsAvailable::add");
  pContext->setExecutionState(LOG_MSG);

  volatile long currentQueue = ::InterlockedIncrement(&m_currentQueue) % m_queueCount;

  {
    ALock lock(m_Queues.at(currentQueue)->sync);
    
    //a_Start timeout timer
    pContext->useTimeoutTimer().start();
    m_Queues.at(currentQueue)->queue.push_back(pContext);
    ++m_Queues.at(currentQueue)->count;
    pContext->setExecutionState(ARope(getClass())+"::add["+AString::fromInt(currentQueue)+"]="+AString::fromPointer(pContext));
  }
}

AOSContext *AOSContextQueue_IsAvailable::_nextContext()
{
  //a_The way the queues work, this method is never used, each queue owns their own set of contexts for select
  ATHROW(this, AException::ProgrammingError);
}
