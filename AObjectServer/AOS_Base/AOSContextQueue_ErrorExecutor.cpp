#include "pchAOS_Base.hpp"
#include "AOSContextQueue_ErrorExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"

const AString& AOSContextQueue_ErrorExecutor::getClass() const
{
  static const AString CLASS("AOSContextQueue_ErrorExecutor");
  return CLASS;
}

void AOSContextQueue_ErrorExecutor::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  BASECLASS_AOSContextQueue_ErrorExecutor::addAdminXml(eBase, request);
}

void AOSContextQueue_ErrorExecutor::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  BASECLASS_AOSContextQueue_ErrorExecutor::processAdminAction(eBase, request);
}

AOSContextQueue_ErrorExecutor::AOSContextQueue_ErrorExecutor(
  AOSServices& services,
  size_t threadCount,              // = 3
  size_t queueCount,               // = 2
  AOSContextQueueInterface *pYes,  // = NULL 
  AOSContextQueueInterface *pNo,   // = NULL
  AOSContextQueueInterface *pError // = NULL
) :
  BASECLASS_AOSContextQueue_ErrorExecutor(services, threadCount, queueCount, pYes, pNo, pError)
{
  useThreadPool().setThis(this);
  registerAdminObject(m_Services.useAdminRegistry());
}

u4 AOSContextQueue_ErrorExecutor::_threadproc(AThread& thread)
{
  AOSContextQueue_ErrorExecutor *pThis = dynamic_cast<AOSContextQueue_ErrorExecutor *>(thread.getThis());
  AASSERT(NULL, pThis);
  AOSContext *pContext = NULL;

  thread.setRunning(true);
  while(thread.isRun())
  {
    try
    {
      while (pContext = pThis->_nextContext()) 
      {
#ifdef __DEBUG_DUMP__
        if (!ADebugDumpable::isPointerValid(pContext))
        {
          AString error("AOSContext pointer is invalid: ");
          error.append(AString::fromPointer(pContext));
          AASSERT_EX(NULL, false, error);
          continue;
        }
#endif
        pContext->setExecutionState(ASW("AOSContextQueue_ErrorExecutor: Processing error condition", 57));

        //a_Should only be here if an error occured, if status not set >200, then assume 500
        if (pContext->useResponseHeader().getStatusCode() == AHTTPResponseHeader::SC_200_Ok)
          pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_500_Internal_Server_Error);

        //a_Check is socket was closed, if so do nothing else, we are done
        if (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_SOCKET_ERROR))
        {
          //a_Proceed
          pThis->_goTerminate(pContext);
          pContext = NULL;
          continue;
        }

        if (
             pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT)
          || pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT)
        )
        {
          //a_Log to file, output is done already
          m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
        }
        else
        {          

          //a_Add XSLT stylesheet for the error XML
          //if (m_Services.useConfiguration().exists(ASW("/config/server/error-stylesheet",31)))
          //{
          //  AString errorStylesheet;
          //  m_Services.useConfiguration().emitString(ASW("/config/server/error-stylesheet",31), errorStylesheet);
          //  pContext->useOutputXmlDocument().addInstruction(AXmlInstruction::XML_STYLESHEET)
          //    .addAttribute(ASW("type",4), ASW("text/xsl",8))
          //    .addAttribute(ASW("href",4), errorStylesheet);
          //}

          //a_Add request header to result XML
          if (!pContext->useOutputRootXmlElement().exists(ASW("REQUEST",7)))
            pContext->useRequestHeader().emitXml(pContext->useOutputRootXmlElement().overwriteElement(ASW("REQUEST",7)));
          pContext->useResponseHeader().emitXml(pContext->useOutputRootXmlElement().overwriteElement(ASW("RESPONSE",8)));

          //a_Check if dumpContext is specified to override and emit XML
          int dumpContextLevel = pContext->getDumpContextLevel();
          pContext->dumpContext(dumpContextLevel);
          if (dumpContextLevel > 0)
          {
            //a_Write contents of the output XML instead of output buffer
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, "text/xml");
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_200_Ok);
            pContext->writeOutputBuffer(true);
          }
          else
          {
            //a_Set the current content type as text/html
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/html",9));

            int statusCode = pContext->useResponseHeader().getStatusCode();
            
            AAutoPtr<ATemplate> pTemplate;  //a_Call to cache manager will set a template
            pContext->useOutputBuffer().clear();
            if (m_Services.useCacheManager().getStatusTemplate(statusCode, pTemplate))
            {
              //a_Template for this status code is found, so process and emit into output buffer
              ABasePtrHolder objects;
              objects.insert(ATemplate::OBJECTNAME_MODEL, &pContext->useOutputXmlDocument());
              objects.insert(AOSContext::OBJECTNAME, pContext);
              pTemplate->process(objects, pContext->useOutputBuffer(), true);
              pContext->setExecutionState(ARope("Using error template for status ")+AString::fromInt(statusCode));
            }
            else
            {
              pContext->setExecutionState(ARope("Did not find error template for status ")+AString::fromInt(statusCode));
            }

            if (pContext->useOutputBuffer().isEmpty())
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

            //a_Write output buffer
            pContext->writeOutputBuffer();
          }
        }

        //a_Close connection
        pContext->useSocket().close();

        //a_Proceed
        pThis->_goYes(pContext);
      }
      AThread::sleep(pThis->m_SleepDelay);  //a_Empty queue, avoid thrashing
    }
    catch(AException& e)
    {
      pContext->setExecutionState(e);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      pThis->_goError(pContext);
      pContext = NULL;
    }
    catch(std::exception& e)
    {
      pContext->setExecutionState(ASWNL(e.what()), true);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      pThis->_goError(pContext);
      pContext = NULL;
    }
    catch(...)
    {
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      pContext->setExecutionState(ASWNL("Unknown exception caught in AOSContextQueue_ErrorExecutor::_threadproc"), true);
      pThis->_goError(pContext);
      pContext = NULL;
    }
  }

  thread.setRunning(false);
  return 0;
}
