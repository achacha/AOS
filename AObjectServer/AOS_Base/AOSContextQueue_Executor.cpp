#include "pchAOS_Base.hpp"
#include "AOSContextQueue_Executor.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "AZlib.hpp"

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
          AString str(1536, 1024);

          //a_Add some common response header pairs
          ATime timeNow;
          timeNow.emitRFCtime(str);
          pContext->useResponseHeader().setPair(AHTTPHeader::HT_GEN_Date, str);

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

        //If output module set this to true, it handled all the output and we don't need to do anything else
        //dumpContext flag will override the output
        if (!pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
        {
          pContext->setExecutionState(ASW("Generating output",17));

          AString strDeflateLevel;
          AString str;
          pContext->useRequestHeader().getPairValue(AHTTPHeader::HT_REQ_Accept_Encoding, str);
          if ( 
               AConstant::npos != str.findNoCase(ASW("deflate",7)) 
               && pContext->useRequestParameterPairs().get(ASW("deflate",7), strDeflateLevel)
             )
          {
            pContext->setExecutionState(ASW("Compressing result",18));
            AString compressed;
            int gzipLevel = strDeflateLevel.toInt();
            if (gzipLevel < 1 || gzipLevel > 9)
              AZlib::deflate(pContext->useOutputBuffer(), compressed);
            else
              AZlib::deflate(pContext->useOutputBuffer(), compressed, gzipLevel);

            pContext->setExecutionState(ASW("Sending compressed result",25));
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Encoding, ASW("deflate",7));
            //pContext->useResponseHeader().setPair(AHTTPHeader::HT_GEN_Transfer_Encoding, ASW("chunked",7));
            //pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Encoding, ASW("gzip",4));
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_RES_Vary, ASW("Accept-Encoding",15));
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(compressed.getSize()));

            //a_The writing of the output
            try
            {
              pContext->writeOutputBuffer(compressed);
            }
            catch(AException& ex)
            {
              pContext->useEventVisitor().set(ex);
              pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
              continue;
            }
          }
          else
          {
            pContext->setExecutionState(ASW("Sending result",14));
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(pContext->useOutputBuffer().getSize()));
            
            //a_Log reponse header to event visitor
            {
              ARope rope("HTTP response header:\r\n",23);
              pContext->useResponseHeader().emit(rope);
              pContext->useEventVisitor().set(rope);
            }

            //a_The writing of the output
            try
            {
              pContext->writeOutputBuffer();
            }
            catch(AException& ex)
            {
              pContext->useEventVisitor().set(ex);
              pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
              continue;
            }
          }
        }
        pContext->useEventVisitor().reset();

        if (pContext->useEventVisitor().getErrorCount() > 0)
        {
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          continue;
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
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_IS_AVAILABLE, &pContext);
        }
        else
        {
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
