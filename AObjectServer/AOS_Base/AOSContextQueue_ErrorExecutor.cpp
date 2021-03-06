/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Base.hpp"
#include "AOSContextQueue_ErrorExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "ASocketException.hpp"

const AString AOSContextQueue_ErrorExecutor::CLASS("AOSContextQueue_ErrorExecutor");

const AString& AOSContextQueue_ErrorExecutor::getClass() const
{
  return CLASS;
}

void AOSContextQueue_ErrorExecutor::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueThreadPool_RoundRobinSwarm::adminEmitXml(eBase, request);
}

void AOSContextQueue_ErrorExecutor::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  AOSContextQueueThreadPool_RoundRobinSwarm::adminProcessAction(eBase, request);
}

AOSContextQueue_ErrorExecutor::AOSContextQueue_ErrorExecutor(
  AOSServices& services,
  size_t threadCount,              // = 3
  size_t queueCount                // = 2
) :
  AOSContextQueueThreadPool_RoundRobinSwarm(services, threadCount, queueCount)
{
  useThreadPool().setThis(this);
  adminRegisterObject(m_Services.useAdminRegistry());
}

void AOSContextQueue_ErrorExecutor::start()
{
  useThreadPool().start();
}

void AOSContextQueue_ErrorExecutor::stop()
{
  useThreadPool().stop();
}

u4 AOSContextQueue_ErrorExecutor::_threadproc(AThread& thread)
{
  AOSContextQueue_ErrorExecutor *pThis = dynamic_cast<AOSContextQueue_ErrorExecutor *>(thread.getThis());
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
        pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_ErrorExecutor: Processing error condition", 57));

        //a_Should only be here if an error occured, if status not set >200, then assume 500
        if (pContext->useResponseHeader().getStatusCode() == AHTTPResponseHeader::SC_200_Ok)
          pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_500_Internal_Server_Error);

        //a_Check is socket was closed, if so do nothing else, we are done
        if (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_SOCKET_ERROR))
        {
          //a_Proceed
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
          continue;
        }

        if (
             pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT)
          || pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT)
        )
        {
          //a_Log to file, output is done already
          m_Services.useLog().add(pContext->useEventVisitor(), ALog::EVENT_FAILURE);
        }
        else
        {          

          //a_Add XSLT stylesheet for the error XML
          //if (m_Services.useConfiguration().exists(ASW("/config/server/error-stylesheet",31)))
          //{
          //  AString errorStylesheet;
          //  m_Services.useConfiguration().emitString(ASW("/config/server/error-stylesheet",31), errorStylesheet);
          //  pContext->useModelXmlDocument().addInstruction(AXmlInstruction::XML_STYLESHEET)
          //    .addAttribute(ASW("type",4), ASW("text/xsl",8))
          //    .addAttribute(ASW("href",4), errorStylesheet);
          //}

          //a_Add request header to result XML
          if (!pContext->useModel().exists(ASW("REQUEST",7)))
            pContext->useRequestHeader().emitXml(pContext->useModel().overwriteElement(ASW("REQUEST",7)));
          pContext->useResponseHeader().emitXml(pContext->useModel().overwriteElement(ASW("RESPONSE",8)));

          //a_Check if dumpContext is specified to override and emit XML
          int dumpContextLevel = pContext->getDumpContextLevel();
          pContext->dumpContext(dumpContextLevel);
          if (dumpContextLevel > 0)
          {
            //a_Write contents of the output XML instead of output buffer
            m_Services.useConfiguration().setMimeTypeFromExt(ASW("xml",3), *pContext);
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_200_Ok);
            pContext->writeOutputBuffer(true);
          }
          else
          {
            //a_Set the current content type as text/html
            pContext->useResponseHeader().set(AHTTPHeader::HT_ENT_Content_Type, ASW("text/html; charset=utf-8",24));

            int statusCode = pContext->useResponseHeader().getStatusCode();
            
            AAutoPtr<ATemplate> pTemplate(NULL, false);  //a_Call to cache manager will set a template
            pContext->clearOutputBuffer();
            if (m_Services.useCacheManager().getStatusTemplate(statusCode, pTemplate))
            {
              //a_Template for this status code is found, so process and emit into output buffer
              pTemplate->process(pContext->useLuaTemplateContext(), pContext->useOutputBuffer());
              if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
              {
                ARope rope("Using error template for status ");
                rope.append(AString::fromInt(statusCode));
                pContext->useEventVisitor().startEvent(rope, AEventVisitor::EL_DEBUG);
              }
            }
            else
            {
              if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_WARN))
              {
                ARope rope("Did not find error template for status ");
                rope.append(AString::fromInt(statusCode));
                pContext->useEventVisitor().startEvent(rope, AEventVisitor::EL_WARN);
              }
            }

            if (pContext->isOutputBufferEmpty())
            {
              AString strError(1024, 256);
              strError.assign("Error ",6);
              strError.append(AString::fromInt(pContext->useResponseHeader().getStatusCode()));
              strError.append(": ", 2);
              strError.append(pContext->useResponseHeader().getStatusCodeReasonPhrase(pContext->useResponseHeader().getStatusCode()));

              //a_Put some generic stuff since there is no error template
              pContext->useOutputBuffer().append("<html><head><title>",19);
              pContext->useOutputBuffer().append(strError);
              pContext->useOutputBuffer().append("</title></head>",15);
              pContext->useOutputBuffer().append("<body>",6);
              pContext->useOutputBuffer().append(strError);
              pContext->useOutputBuffer().append("</body></html>",14);
            }

            try
            {
              pContext->writeOutputBuffer();
            }
            catch(ASocketException& ex)
            {
              pContext->useEventVisitor().addEvent(ex, AEventVisitor::EL_ERROR);
              pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
              continue;
            }
          }
        }

        //a_Close connection
        pContext->useSocket().close();

        //a_Proceed
        m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
        continue;
      }
      AThread::sleep(pThis->m_SleepDelay);  //a_Empty queue, avoid thrashing
    }
    catch(AException& e)
    {
      pContext->useEventVisitor().addEvent(e, AEventVisitor::EL_ERROR);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::EVENT_FAILURE);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
    }
    catch(std::exception& e)
    {
      pContext->useEventVisitor().addEvent(ASWNL(e.what()), AEventVisitor::EL_ERROR);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::EVENT_FAILURE);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
    }
    catch(...)
    {
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::EVENT_FAILURE);
      pContext->useEventVisitor().addEvent(ASWNL("Unknown exception caught in AOSContextQueue_ErrorExecutor::_threadproc"), AEventVisitor::EL_ERROR);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
      break;
    }
  }

  thread.setRunning(false);
  return 0;
}
