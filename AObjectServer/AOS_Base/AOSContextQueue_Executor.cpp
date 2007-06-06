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

void AOSContextQueue_Executor::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  BASECLASS_AOSContextQueue_Executor::addAdminXml(eBase, request);
}

void AOSContextQueue_Executor::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  BASECLASS_AOSContextQueue_Executor::processAdminAction(eBase, request);
}

AOSContextQueue_Executor::AOSContextQueue_Executor(
  AOSServices& services,
  size_t threadCount,               // = 12
  size_t queueCount,                // = 3
  AOSContextQueueInterface *pYes,   // = NULL 
  AOSContextQueueInterface *pNo,    // = NULL
  AOSContextQueueInterface *pError  // = NULL
) :
  BASECLASS_AOSContextQueue_Executor(services, threadCount, queueCount, pYes, pNo, pError),
  m_InputExecutor(services),
  m_ModuleExecutor(services),
  m_OutputExecutor(services)
{
  useThreadPool().setParameter(this);
  registerAdminObject(m_Services.useAdminRegistry());
}

u4 AOSContextQueue_Executor::_threadproc(AThread& thread)
{
  AOSContextQueue_Executor *pThis = static_cast<AOSContextQueue_Executor *>(thread.getParameter());
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

        //
        //a_Process input
        //
        pThis->m_InputExecutor.execute(*pContext);

        if (pContext->useEventVisitor().getErrorCount() > 0)
        {
          pThis->_goError(pContext);
          continue;
        }

        //
        //a_Process modules
        //
        pThis->m_ModuleExecutor.execute(*pContext);

        if (pContext->useEventVisitor().getErrorCount() > 0)
        {
          pThis->_goError(pContext);
          continue;
        }

        //
        //a_Prepare for output
        //
        AString str(1536, 1024);
        {
          //a_Add some common response header pairs
          ATime timeNow;
          timeNow.emitRFCtime(str);
          pContext->useResponseHeader().setPair(AHTTPHeader::HT_GEN_Date, str);
        }

        int dumpContext = 0;
        str.clear();
        if (pContext->useRequestParameterPairs().get(ASW("dumpContext", 11), str))
        {
          dumpContext = str.toInt();
        }

        //
        //a_Generate output
        //
        pContext->setExecutionState(ASW("Executing output generator",26));
        pThis->m_OutputExecutor.execute(*pContext);

        if (dumpContext > 0)
        {
          //a_Dump context as XML instead of usual output
          pContext->useOutputRootXmlElement().addElement(ASW("/context/dump",13), *pContext);
          pContext->useOutputRootXmlElement().addElement(ASW("/context/buffer",15), pContext->useOutputBuffer(), AXmlData::CDataHexDump);

          if (dumpContext > 1)
          {
            pContext->useOutputRootXmlElement().addElement(ASW("/context/buffer",15), pContext->useServices().useConfiguration().getConfigRoot());
          }

          //a_Clear the output buffer and force type for be XML, code below will emit the doc into buffer
          pContext->useOutputBuffer().clear();
        }

        //
        //a_Publish errors if any
        //
        if (!pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT))
        {
          pContext->setExecutionState(ASW("Emitting event visitor",22));
          if (pContext->useEventVisitor().getErrorCount() > 0)
          {
            pContext->useEventVisitor().emit(pContext->useOutputRootXmlElement().addElement("AEventVisitor"));
          }
        }

        //a_If output module set this to true, it handled all the output and we don't need to do anything else
        //a_dumpContext flag will override the output
        if (!pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT) || dumpContext > 0)
        {
          pContext->setExecutionState(ASW("Generating output",17));

          AString strDeflateLevel;
          str.clear();
          pContext->useRequestHeader().find(AHTTPHeader::HT_REQ_Accept_Encoding, str);
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
            //pContext->useResponseHeader().setPair(AHTTPHeader::HT_RES_Vary, ASW("Accept-Encoding",15));
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(compressed.getSize()));

            //a_The writing of the output
            try
            {
              pContext->writeOutputBuffer(compressed);
            }
            catch(...)
            {
              pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
              throw;
            }
          }
          else
          {
            pContext->setExecutionState(ASW("Sending result",14));
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(pContext->useOutputBuffer().getSize()));

            //a_The writing of the output
            try
            {
              pContext->writeOutputBuffer();
            }
            catch(...)
            {
              pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
              throw;
            }
          }
        }
        pContext->useEventVisitor().reset();

        if (pContext->useEventVisitor().getErrorCount() > 0)
        {
          pThis->_goError(pContext);
          continue;
        }
        
        //a_Force a close connection if output got committed and the content-length was not set 
        if (
             pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_OUTPUT_SENT)
             && !pContext->useResponseHeader().exists(AHTTPHeader::HT_ENT_Content_Length)
        )
        {
          pContext->setExecutionState(ASW("AOSContextQueue_Executor: Forcing a close since response Content-Length was not specified",89));
          pThis->_goTerminate(pContext);
          continue;
        }

        if (
          pContext->useRequestHeader().isHttpPipeliningEnabled()
        )
        {
          //a_keep-alive found, pipelining enabled
          pThis->_goYes(pContext);
        }
        else
        {
          pContext->useSocket().close();
          pThis->_goNo(pContext);
        }
      }
      AThread::sleep(3);  //a_Empty queue, avoid thrashing
    }
    catch(AException& e)
    {
      pContext->setExecutionState(e);
      _goError(pContext);
    }
    catch(...)
    {
      pContext->setExecutionState(ASW("Unknown exception caught in AOSContextQueue_Executor::threadproc",64), true);
      _goError(pContext);
    }
  }

  thread.setRunning(false);
  return 0;
}

AOSInputExecutor& AOSContextQueue_Executor::useAOSInputExecutor() 
{ 
  return m_InputExecutor;
}

AOSModuleExecutor& AOSContextQueue_Executor::useAOSModuleExecutor()
{ 
  return m_ModuleExecutor; 
}

AOSOutputExecutor& AOSContextQueue_Executor::useAOSOutputExecutor()
{ 
  return m_OutputExecutor;
}
