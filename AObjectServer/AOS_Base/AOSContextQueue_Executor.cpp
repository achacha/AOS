#include "pchAOS_Base.hpp"
#include "AOSContextQueue_Executor.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "ASocketException.hpp"

const AString& AOSContextQueue_Executor::getClass() const
{
  static const AString CLASS("AOSContextQueue_Executor");
  return CLASS;
}

void AOSContextQueue_Executor::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  BASECLASS_AOSContextQueue_Executor::adminEmitXml(eBase, request);
}

void AOSContextQueue_Executor::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  BASECLASS_AOSContextQueue_Executor::adminProcessAction(eBase, request);
}

AOSContextQueue_Executor::AOSContextQueue_Executor(
  AOSServices& services,
  size_t threadCount,               // = 12
  size_t queueCount                 // = 3
) :
  BASECLASS_AOSContextQueue_Executor(services, threadCount, queueCount)
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
        //a_Process input
        //
        pThis->m_Services.useInputExecutor().execute(*pContext);

        //a_If error is logged stop and go to error handler
        if (pContext->useEventVisitor().getErrorCount() > 0)
        {
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          continue;
        }

        //
        // Process modules
        // Unless we are in redirect mode
        //
        if (pContext->useContextFlags().isClear(AOSContext::CTXFLAG_IS_REDIRECTING) &&  pContext->getCommand())
          pThis->m_Services.useModuleExecutor().execute(*pContext, pContext->getCommand()->getModules());
        else
          pContext->useEventVisitor().set(ASW("Command not found, skipping module execution.",45), true);

        //a_If error is logged stop and go to error handler
        if (pContext->useEventVisitor().getErrorCount() > 0)
        {
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          continue;
        }

        //
        // Prepare for output
        //
        if (pContext->useContextFlags().isClear(AOSContext::CTXFLAG_IS_REDIRECTING))
        {
          int dumpContextLevel = pContext->getDumpContextLevel();

          //a_Add REQUEST, SESSION and RESPONSE header only if not in AJAX mode
          if (pContext->useContextFlags().isClear(AOSContext::CTXFLAG_IS_AJAX))
          {
            pContext->useRequestHeader().emitXml(pContext->useModel().overwriteElement(ASW("REQUEST",7)));
            pContext->useResponseHeader().emitXml(pContext->useModel().overwriteElement(ASW("RESPONSE",8)));
            pContext->useSessionData().emitXml(pContext->useModel().overwriteElement(ASW("SESSION",7)));
          }

          //
          // Generate output
          // Unless redirecting
          //
          pContext->setExecutionState(ASW("Executing output generator",26));
          pThis->m_Services.useOutputExecutor().execute(*pContext);

          //a_Dump context as XML instead of usual output
          pContext->dumpContext(dumpContextLevel);
          if (dumpContextLevel > 0)
          {
            //a_Clear the output buffer and force type for be XML, code below will emit the doc into buffer
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, "text/xml");
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_200_Ok);
            pContext->writeOutputBuffer(true);
          }

          //a_Publish total execution time
          if (!pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_AJAX))
          {
            pContext->useModel().addElement(ASW("total_time",10), pContext->getContextTimer());
          }

          //
          //a_Publish errors if any
          //
          if (!pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
          {
            pContext->setExecutionState(ASW("Emitting event visitor",22));
            if (pContext->useEventVisitor().getErrorCount() > 0)
            {
              pContext->useEventVisitor().emitXml(pContext->useModel().addElement("AEventVisitor"));
            }
          }
        }

        if (!pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
        {
          try
          {
            pContext->writeOutputBuffer();
          }
          catch(ASocketException& ex)
          {
            pContext->useEventVisitor().set(ex);
            pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
            continue;
          }
        }
        else
        {
          pContext->setExecutionState(ASW("AOSContextQueue_Executor: Output already sent",45));
        }

        //a_Force a close connection if output got committed and the content-length was not set 
        if (
             pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT)
             && !pContext->useResponseHeader().exists(AHTTPHeader::HT_ENT_Content_Length)
        )
        {
          pContext->setExecutionState(ASW("AOSContextQueue_Executor: Forcing a close since response Content-Length was not specified",89));
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
          continue;
        }

        if (pContext->useRequestHeader().isHttpPipeliningEnabled())
        {
          //a_keep-alive found, pipelining enabled
          pContext->setExecutionState(ASW("AOSContextQueue_Executor: Pipelining detected, going into preExecute",68));
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_PRE_EXECUTE, &pContext);
        }
        else
        {
          //a_No pipelining
          pContext->setExecutionState(ASW("AOSContextQueue_Executor: Pipelining not detected, terminating request",70));
          pContext->useSocket().close();
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
        }
        continue;
      }
      AThread::sleep(pThis->m_SleepDelay);  //a_Empty queue, avoid thrashing
    }
    catch(AException& e)
    {
      pContext->setExecutionState(e, true);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
    }
    catch(std::exception& e)
    {
      pContext->setExecutionState(ASWNL(e.what()), true);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
    }
    catch(...)
    {
      pContext->setExecutionState(ASW("Unknown exception caught in AOSContextQueue_Executor::threadproc",64), true);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
      break;
    }
  }

  thread.setRunning(false);
  return 0;
}
