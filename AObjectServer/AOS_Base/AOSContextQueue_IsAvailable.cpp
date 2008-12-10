/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSContextQueue_IsAvailable.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "AThread.hpp"

const AString AOSContextQueue_IsAvailable::CLASS("AOSContextQueue_IsAvailable");

const AString& AOSContextQueue_IsAvailable::getClass() const
{
  return CLASS;
}

void AOSContextQueue_IsAvailable::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;

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

  m_NoDataTimeout = m_Services.useConfiguration().getConfigRoot().getInt(ASW("/config/server/context-queue/is-available/no-data-timeout",57), 60000);
  m_EmptyQueueDelay = m_Services.useConfiguration().getConfigRoot().getInt(ASW("/config/server/context-queue/is-available/empty-queue-sleep-delay",65), 20);
  m_LoopDelay = m_Services.useConfiguration().getConfigRoot().getInt(ASW("/config/server/context-queue/is-available/loop-delay",52), 5);

  adminRegisterObject(services.useAdminRegistry());
}

AOSContextQueue_IsAvailable::~AOSContextQueue_IsAvailable()
{
  try
  {
    stop();
    for (size_t i=0; i<m_Queues.size(); ++i)
    {
      m_Queues.at(i)->queue.clear(true);
      delete m_Queues.at(i);
    }
  }
  catch(...) {}
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
    ASW("NoDataTimeout",13),
    AString::fromInt(m_NoDataTimeout)
  );

  adminAddProperty(
    eBase,
    ASW("EmptyQueueDelay",15),
    AString::fromInt(m_EmptyQueueDelay)
  );

  adminAddProperty(
    eBase,
    ASW("LoopDelay",9),
    AString::fromInt(m_LoopDelay)
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
  queue.clear(true);
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
    if (m_Queues.at(i)->pthread)
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

    // Everything stopped
    if (i == m_Queues.size())
      return;

    // Time to terminate the threads
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

  thread.setRunning(true);
  fd_set sockSet;
  while (thread.isRun())
  {
    try
    {
      if (pThis->queue.size() > 0)
      {
        FD_ZERO(&sockSet);
        
        int count = 0;
        // Convert to ABasePtrQueue
        ABase *p = pThis->queue.useHead();
        while (count < FD_SETSIZE && p)
        {
          AOSContext *pContext = (AOSContext *)p;
          FD_SET((SOCKET)pContext->useSocket().getSocketInfo().m_handle, &sockSet);
          ++count;
          p = p->useNext();
        }
        
        if (count > 0)
        {
          int availCount = ::select(count, &sockSet, NULL, NULL, &timeout);
          if (availCount > 0)
          {
            int count2 = 0;
            p = pThis->queue.useHead();

            while (availCount > 0 && count2 <= count && p)
            {
              AOSContext *pContext = (AOSContext *)p;
              p = p->useNext();
              if (FD_ISSET(pContext->useSocket().getSocketInfo().m_handle, &sockSet))
              {
                pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_ISAVAILABLE_SELECT_SET);

                //a_Attempt to read some data to check if it actually has data, if not then connection is closed
                size_t bytesRead = AConstant::npos;
                try
                {
                  bytesRead = pContext->useSocket().readBlockIntoLookahead();
                }
                catch(AException e)
                {
                  bytesRead = AConstant::npos;  //a_Following switch will handle this
                  pContext->useEventVisitor().addEvent(ASW("AOSContextQueue_IsAvailable: Exception reading from socket",58), AEventVisitor::EL_ERROR);
                }

                switch(bytesRead)
                {
                  case AConstant::npos:
                  case 0:
                  {
                    //a_No data to read, socket is closed
                    pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_CLOSED);
                    pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_IsAvailable: Detected closed remote socket",58), AEventVisitor::EL_WARN);
                    pContext->useSocket().close();

                    {
                      ALock lock(pThis->sync);
                      pThis->queue.remove(pContext);
                    }
                    pOwner->m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
                    
                  }
                  break;

                  case AConstant::unavail:
                    if (pContext->useTimeoutTimer().getInterval() > pOwner->m_NoDataTimeout)
                    {
                      AString str;
                      str.append("AOSContextQueue_IsAvailable: No data (non-blocking) after timeout: ",67);
                      pContext->useTimeoutTimer().emit(str);

                      //a_We are done with this request, still no data
                      pContext->useEventVisitor().startEvent(
                        str, 
                        (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING) ? AEventVisitor::EL_EVENT : AEventVisitor::EL_ERROR)
                      );
                      {
                        ALock lock(pThis->sync);
                        pThis->queue.remove(pContext);
                      }
                      pOwner->m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
                    }
                  break;  // Would block, keep waiting

                  default:
                  {
                    //a_Add to next queue
                    pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_ISAVAILABLE_PENDING);
                    if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
                      pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_IsAvailable: HTTP header has more data",54), AEventVisitor::EL_DEBUG);

                    {
                      ALock lock(pThis->sync);
                      pThis->queue.remove(pContext);
                    }
                    pOwner->m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_PRE_EXECUTE, &pContext);
                  }
                }

                --availCount;
              }
              else
              {
                //a_Select called on this AOSContext and no data available
                if (pContext->useTimeoutTimer().getInterval() > pOwner->m_NoDataTimeout)
                {
                  AString str;
                  str.append("AOSContextQueue_IsAvailable: No data after timeout: ",52);
                  pContext->useTimeoutTimer().emit(str);

                  //a_We are done with this request, still no data
                  pContext->useEventVisitor().startEvent(
                    str, 
                    (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING) ? AEventVisitor::EL_EVENT : AEventVisitor::EL_ERROR)
                  );
                  {
                    ALock lock(pThis->sync);
                    pThis->queue.remove(pContext);
                  }
                  pOwner->m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
                }
              }
              ++count2;
            }
            AASSERT(pOwner, !availCount);
          }
          else
          {
            //a_Nothing has data, flag them all
            p = pThis->queue.useHead();
            while (p)
            {
              AOSContext *pContext = (AOSContext *)p;
              p = p->useNext();
              if (pContext->useTimeoutTimer().getInterval() > pOwner->m_NoDataTimeout)
              {
                AString str("AOSContextQueue_IsAvailable: No data after timeout: ",52);
                pContext->useTimeoutTimer().emit(str);

                //a_We are done with this request, still no data
                ALock lock(pThis->sync);
                pContext->useEventVisitor().startEvent(
                  str, 
                  (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING) ? AEventVisitor::EL_EVENT : AEventVisitor::EL_ERROR)
                );
                pThis->queue.remove(pContext);
                pOwner->m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
              }
            }
          }
        }
        
        //a_Sleep between selects, requests in this queue are already slow
        AThread::sleep(pOwner->m_LoopDelay);
      }
      else
        AThread::sleep(pOwner->m_EmptyQueueDelay);    //a_Nothing in queue, deep sleep
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
  AASSERT(NULL, pContext);
  
  //a_Clear these in case this is not the first trip here
  //a_More than one trip can result if some bytes were read but not enough
  pContext->useConnectionFlags().clearBit(AOSContext::CONFLAG_ISAVAILABLE_SELECT_SET);
  pContext->useConnectionFlags().clearBit(AOSContext::CONFLAG_ISAVAILABLE_PENDING);

  volatile long currentQueue = ::InterlockedIncrement(&m_currentQueue) % m_queueCount;

  {
    ALock lock(m_Queues.at(currentQueue)->sync);
    
    //a_Start timeout timer
    pContext->useTimeoutTimer().start();
    if (pContext->useEventVisitor().isLoggingDebug())
    {
      ARope rope(getClass());
      rope.append("::add[",6);
      rope.append(AString::fromInt(currentQueue));
      rope.append("]=",2);
      rope.append(AString::fromPointer(pContext));
      pContext->useEventVisitor().addEvent(rope, AEventVisitor::EL_DEBUG);
    }

    m_Queues.at(currentQueue)->queue.pushBack(pContext);
    ++m_Queues.at(currentQueue)->count;
  }
}

AOSContext *AOSContextQueue_IsAvailable::_nextContext()
{
  //a_The way the queues work, this method is never used, each queue owns their own set of contexts for select
  ATHROW(this, AException::ProgrammingError);
}
