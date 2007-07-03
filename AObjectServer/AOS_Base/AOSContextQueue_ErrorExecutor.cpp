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
  useThreadPool().setParameter(this);
  registerAdminObject(m_Services.useAdminRegistry());
}

u4 AOSContextQueue_ErrorExecutor::_threadproc(AThread& thread)
{
  AOSContextQueue_ErrorExecutor *pThis = static_cast<AOSContextQueue_ErrorExecutor *>(thread.getParameter());
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
          AASSERT_EX(NULL, false, error.c_str());
          continue;
        }
#endif
        //a_Should only be here if an error occured
        AASSERT(pThis, pContext->useEventVisitor().getErrorCount() > 0);

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
          //a_Process and display error
          AXmlElement& eError = pContext->useOutputRootXmlElement().addElement(ASW("error", 5));
          pContext->useEventVisitor().emit(eError);
          
          //a_Prepare result
          pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, "text/html");
          pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_500_Internal_Server_Error);

          AAutoPtr<ATemplate> pTemplate;
          if (m_Services.useCacheManager().getStatusTemplate(500, pTemplate))
          {
            //a_Template for this status code is found, so process and emit into output buffer
            pContext->useOutputBuffer().clear();
            pTemplate->process(pContext->useOutputBuffer(), pContext->useOutputRootXmlElement());
          }

          //a_Add XSLT stylesheet for the error XML
          //if (m_Services.useConfiguration().exists(ASW("/config/server/error-stylesheet",31)))
          //{
          //  AString errorStylesheet;
          //  m_Services.useConfiguration().emitString(ASW("/config/server/error-stylesheet",31), errorStylesheet);
          //  pContext->useOutputXmlDocument().addInstruction(AXmlInstruction::XML_STYLESHEET)
          //    .addAttribute(ASW("type",4), ASW("text/xsl",8))
          //    .addAttribute(ASW("href",4), errorStylesheet);
          //}

          if (pContext->useOutputBuffer().isEmpty())
          {
            //a_Put some generic stuff since there is no error template
            pContext->useOutputBuffer().append("<html><head><title>Error 500: Internal Server Error</title></head>");
            pContext->useOutputBuffer().append("<body>Error 500: Internal Server Error</body></html>");
          }

          //a_Emit HTTP result
          int dumpContext = 0;
          AString str;
          if (pContext->useRequestParameterPairs().get(ASW("dumpContext", 11), str))
          {
            dumpContext = str.toInt();
          }
          pContext->writeOutputBuffer(dumpContext > 0 ? true : false);
        }

        //a_Close connection
        pContext->useSocket().close();

        //a_Proceed
        pThis->_goYes(pContext);
      }
      AThread::sleep(20);  //a_Empty queue, avoid thrashing
    }
    catch(AException& e)
    {
      pContext->setExecutionState(e);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      pThis->_goError(pContext);
    }
    catch(...)
    {
      pContext->setExecutionState(ASWNL("Unknown exception caught in AOSContextQueue_ErrorExecutor::_threadproc"), true);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      pThis->_goError(pContext);
    }
  }

  thread.setRunning(false);
  return 0;
}
