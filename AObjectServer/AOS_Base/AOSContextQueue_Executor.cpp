/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSContextQueue_Executor.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "ASocketException.hpp"

const AString AOSContextQueue_Executor::CLASS("AOSContextQueue_Executor");

const AString& AOSContextQueue_Executor::getClass() const
{
  return CLASS;
}

void AOSContextQueue_Executor::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueThreadPool_RoundRobinSwarm::adminEmitXml(eBase, request);
}

void AOSContextQueue_Executor::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueThreadPool_RoundRobinSwarm::adminProcessAction(eBase, request);
}

AOSContextQueue_Executor::AOSContextQueue_Executor(
  AOSServices& services,
  size_t threadCount,               // = 12
  size_t queueCount                 // = 3
) :
  AOSContextQueueThreadPool_RoundRobinSwarm(services, threadCount, queueCount)
{
  useThreadPool().setThis(this);
  adminRegisterObject(m_Services.useAdminRegistry());
}

void AOSContextQueue_Executor::start()
{
  useThreadPool().start();
}

void AOSContextQueue_Executor::stop()
{
  useThreadPool().stop();
}

u4 AOSContextQueue_Executor::_threadproc(AThread& thread)
{
  AOSContextQueue_Executor *pThis = dynamic_cast<AOSContextQueue_Executor *>(thread.getThis());
  AASSERT(&thread, pThis);
  AOSContext *pContext = NULL;

  thread.setRunning(true);
  while(thread.isRun())
  {
    try
    {
      while (pContext = pThis->_nextContext()) 
      {
#ifndef NDEBUG
        if (!ADebugDumpable::isPointerValid(pContext))
        {
          AString error("AOSContext pointer is invalid: ");
          error.append(AString::fromPointer(pContext));
          AASSERT_EX(NULL, false, error);
          continue;
        }
#endif
        //
        // Process input
        //
        pThis->m_Services.useInputExecutor().execute(*pContext);

        //a_If error is logged stop and go to error handler
        if (pContext->useEventVisitor().getErrorCount() > 0)
        {
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          continue;
        }

        //
        // Add REQUEST header and SESSION data if not in AJAX mode
        //
        if (pContext->useContextFlags().isClear(AOSContext::CTXFLAG_IS_AJAX))
        {
          pContext->useRequestHeader().emitXml(pContext->useModel().overwriteElement(AOSContext::S_REQUEST));

          if (pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_USING_SESSION_DATA))
            pContext->useSessionData().emitXml(pContext->useModel().overwriteElement(AOSContext::S_SESSION));
        }

        //
        // Process modules
        // Unless we are in redirect mode
        //
        if (pContext->useContextFlags().isClear(AOSContext::CTXFLAG_IS_REDIRECTING) &&  pContext->getController())
          pThis->m_Services.useModuleExecutor().execute(*pContext, pContext->getController()->getModules());
        else
          pContext->useEventVisitor().startEvent(ASW("Command not found, skipping module execution.",45), AEventVisitor::EL_ERROR);

        //
        // Add SESSION data again in-case it was modified if not in AJAX mode
        //
        if (pContext->useContextFlags().isClear(AOSContext::CTXFLAG_IS_AJAX))
        {
          if (pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_USING_SESSION_DATA))
          {
            AXmlElement& session = pContext->useModel().overwriteElement(AOSContext::S_SESSION);
            session.clear();
            pContext->useSessionData().emitXml(session);
          }
        }

        //
        // If error is logged stop and go to error handler
        //
        if (pContext->useEventVisitor().getErrorCount() > 0)
        {
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          continue;
        }

        //
        // Prepare for output
        //
        bool forceXml = false;
        if (pContext->useContextFlags().isClear(AOSContext::CTXFLAG_IS_REDIRECTING))
        {
          int dumpContextLevel = pContext->getDumpContextLevel();

          //a_Add SESSION and RESPONSE header only if not in AJAX mode
          if (pContext->useContextFlags().isClear(AOSContext::CTXFLAG_IS_AJAX))
          {
            pContext->useResponseHeader().emitXml(pContext->useModel().overwriteElement(AOSContext::S_RESPONSE));
          }

          //
          // Generate output
          // Unless redirecting
          //
          pContext->useEventVisitor().startEvent(ASW("Executing output generator",26), AEventVisitor::EL_INFO);
          pThis->m_Services.useOutputExecutor().execute(*pContext);
          pContext->useEventVisitor().endEvent();

          //a_Dump context as XML instead of usual output
          if (dumpContextLevel > 0)
          {
            pContext->dumpContext(dumpContextLevel);
            forceXml = true;
          }

          //a_Publish total execution time
          if (pContext->useContextFlags().isClear(AOSContext::CTXFLAG_IS_AJAX))
          {
            pContext->useModel().addElement(ASW("total_time",10), pContext->getContextTimer());
          }

          //
          //a_Publish errors if any
          //
          if (!pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT) && pContext->useEventVisitor().getErrorCount() > 0)
          {
            if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_INFO))
              pContext->useEventVisitor().startEvent(ASW("Emitting event visitor",22), AEventVisitor::EL_INFO);
            pContext->useEventVisitor().emitXml(pContext->useModel().addElement(ASW("AEventVisitor",13)));
            pContext->useEventVisitor().endEvent();
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
            continue;
          }
        }

        //a_If the output was not sent yet, do so
        if (!pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
        {
          try
          {
            pContext->writeOutputBuffer(forceXml);
          }
          catch(ASocketException& ex)
          {
            pContext->useEventVisitor().addEvent(ex, AEventVisitor::EL_ERROR);
            pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
            continue;
          }
        }
        else
        {
          pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_Executor: Output already sent",45), AEventVisitor::EL_ERROR);
        }

        //a_Force a close connection if output got committed and the content-length was not set 
        if (
            pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT)
         && !pContext->useResponseHeader().exists(AHTTPHeader::HT_ENT_Content_Length)
        )
        {
          if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_WARN))
            pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_Executor: Forcing a close since response Content-Length was not specified",89), AEventVisitor::EL_WARN);
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
          continue;
        }

        //a_Connection: Close was specified for request or response
        if (pContext->isConnectionClose())
        {
          if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_INFO))
            pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_Executor: Forcing a close since Connection: Close was detected on request or response",101), AEventVisitor::EL_INFO);
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
          continue;
        }

        if (pContext->useRequestHeader().isHttpPipeliningEnabled())
        {
          //a_keep-alive found, pipelining enabled
          if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_INFO))
            pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_Executor: Pipelining detected, going into preExecute",68), AEventVisitor::EL_INFO);
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_PRE_EXECUTE, &pContext);
        }
        else
        {
          //a_No pipelining
          if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_INFO))
            pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_Executor: Pipelining not detected, terminating request",70), AEventVisitor::EL_INFO);
          pContext->useSocket().close();
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
        }
        continue;
      }
      AThread::sleep(pThis->m_SleepDelay);  //a_Empty queue, avoid thrashing
    }
    catch(AException& e)
    {
      pContext->useEventVisitor().addEvent(e, AEventVisitor::EL_ERROR);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
    }
    catch(std::exception& e)
    {
      pContext->useEventVisitor().addEvent(ASWNL(e.what()), AEventVisitor::EL_ERROR);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
    }
    catch(...)
    {
      pContext->useEventVisitor().addEvent(ASW("Unknown exception caught in AOSContextQueue_Executor::threadproc",64), AEventVisitor::EL_ERROR);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
      break;
    }
  }

  thread.setRunning(false);
  return 0;
}
