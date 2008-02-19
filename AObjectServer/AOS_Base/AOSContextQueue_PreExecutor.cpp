#include "pchAOS_Base.hpp"
#include "AOSContextQueue_PreExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "AOSCommand.hpp"
#include "AOSConfiguration.hpp"
#include "ASocketException.hpp"

const AString& AOSContextQueue_PreExecutor::getClass() const
{
  static const AString CLASS("AOSContextQueue_PreExecutor");
  return CLASS;
}

void AOSContextQueue_PreExecutor::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  BASECLASS_AOSContextQueue_PreExecutor::adminEmitXml(eBase, request);
}

void AOSContextQueue_PreExecutor::adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  BASECLASS_AOSContextQueue_PreExecutor::adminProcessAction(eBase, request);
}

AOSContextQueue_PreExecutor::AOSContextQueue_PreExecutor(
  AOSServices& services,
  size_t threadCount,              // = 16
  size_t queueCount                // = 4
) :
  BASECLASS_AOSContextQueue_PreExecutor(services, threadCount, queueCount)
{
  useThreadPool().setThis(this);
  adminRegisterObject(m_Services.useAdminRegistry());
}

AOSContextQueue_PreExecutor::~AOSContextQueue_PreExecutor()
{
}

void AOSContextQueue_PreExecutor::start()
{
  useThreadPool().start();
}

void AOSContextQueue_PreExecutor::stop()
{
  useThreadPool().stop();
}

u4 AOSContextQueue_PreExecutor::_threadproc(AThread& thread)
{
  AOSContextQueue_PreExecutor *pThis = dynamic_cast<AOSContextQueue_PreExecutor *>(thread.getThis());
  AASSERT(&thread, pThis);
  AOSContext *pContext = NULL;

  const int iWaitForHttpDataTimeout = pThis->m_Services.useConfiguration().useConfigRoot().getInt(ASW("/config/server/http/wait-for-http-data-timeout",46), 10000);
  thread.setRunning(true);
  while(thread.isRun())
  {
    try
    {
      while (
           (pContext && pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING))
        || (pContext = pThis->_nextContext())
      ) 
      {
        AASSERT(this, pContext);

#ifndef NDEBUG
        if (!ADebugDumpable::isPointerValid(pContext))
        {
          AString error("AOSContextQueue_PreExecutor: AOSContext pointer is invalid: ");
          error.append(AString::fromPointer(pContext));
          AASSERT_EX(NULL, false, error);
          pContext = NULL;
          continue;
        }
#endif
        //a_Ack the keep-alive if pipelining is on
        if (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING))
        {
          pContext->useResponseHeader().setPair(AHTTPHeader::HT_GEN_Connection, ASW("keep-alive", 10));
        }

        //a_Try to initialize the context and read the HTTP header
        AASSERT(pContext, !pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_SOCKET_ERROR));
        switch (pContext->init())
        {
          case AOSContext::STATUS_HTTP_INCOMPLETE_NODATA:
            if (pContext->getRequestTimer().getInterval() > iWaitForHttpDataTimeout)
            {
              //a_Waited long enough
              pContext->setExecutionState(ASW("AOSContextQueue_PreExecutor: HTTP header started but did not complete before timeout, abandoning wait",101));
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
              continue;
            }
            else
            {
              //a_No data trying to read first character, go into isAvailable
              pContext->setExecutionState(ASW("AOSContextQueue_PreExecutor: No data yet for HTTP header, going into waiting queue",82));
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_IS_AVAILABLE, &pContext);
            }
          continue;

            //Fallthrough from above
          case AOSContext::STATUS_HTTP_INCOMPLETE_METHOD:
          case AOSContext::STATUS_HTTP_INCOMPLETE_LINEZERO:
          case AOSContext::STATUS_HTTP_INCOMPLETE_CRLFCRLF:
          {
            //a_Go into waiting queue
            pContext->setExecutionState(ASW("AOSContextQueue_PreExecutor: HTTP header not complete, going into waiting queue",79));
            if (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_SOCKET_ERROR))
            {
              //a_Socket error occured, just terminate and keep processing
              pContext->setExecutionState(ASW("AOSContextQueue_PreExecutor: Client socket error detected before heade read, terminating request",96));
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
            }
            else
            {
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_IS_AVAILABLE, &pContext);
            }
          }
          continue;

          case AOSContext::STATUS_HTTP_METHOD_OPTIONS:
          {
            //a_Handle OPTIONS request
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_200_Ok);
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_RES_Public, ASW("OPTIONS, GET, HEAD, POST",24));
            pContext->useResponseHeader().setPair(ASW("Compliance",10), ASW("rfc=1945, rfc=2068",18));
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AConstant::ASTRING_ZERO);

            //a_Send header and no body
            pContext->setExecutionState(ASW("Writing OPTIONS response",24));
            pContext->useResponseHeader().emit(pContext->useSocket());
            
            //a_Flag that all is done
            pContext->useContextFlags().setBit(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT, true);
            pContext->useContextFlags().setBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT, true);
            pContext->useEventVisitor().reset();
            
            //a_Request is done
            pContext->useEventVisitor().set(pContext->useRequestHeader());
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
          }
          continue;

          case AOSContext::STATUS_HTTP_METHOD_NOT_ALLOWED:
          {
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_405_Method_Not_Allowed);
            pContext->useEventVisitor().set(pContext->useRequestHeader());
            pContext->useEventVisitor().reset();
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          }
          continue;

          case AOSContext::STATUS_HTTP_VERSION_NOT_SUPPORTED:
          {
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_505_HTTP_Version_Not_Supported);
            pContext->useEventVisitor().set(pContext->useRequestHeader());
            pContext->useEventVisitor().reset();
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          }
          continue;

          case AOSContext::STATUS_HTTP_UNKNOWN_METHOD:
          {
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_400_Bad_Request);
            pContext->useEventVisitor().set(pContext->useRequestHeader());
            pContext->useEventVisitor().reset();
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          }
          continue;

          case AOSContext::STATUS_HTTP_INVALID_HEADER:
          case AOSContext::STATUS_HTTP_INVALID_REQUEST_PATH:
            pContext->useEventVisitor().set(pContext->useRequestHeader());
          //a_Fallthrough
          case AOSContext::STATUS_HTTP_INVALID_FIRST_CHAR:
          case AOSContext::STATUS_HTTP_INVALID_AFTER_METHOD_CHAR:
          {
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_400_Bad_Request);
            pContext->useEventVisitor().reset();
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          }
          continue;

          case AOSContext::STATUS_HTTP_SOCKET_CLOSED:
          {
            //a_Socket was closed, terminate
            pContext->setExecutionState(ASW("AOSContextQueue_PreExecutor: Socket was closed by client, terminating",69), true);
            pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_CLOSED);
            pContext->useEventVisitor().reset();
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
          }
          continue;

          case AOSContext::STATUS_OK:
          {
            //a_Update the context name
            pContext->useEventVisitor().useName().append("  \r\n",4);
            pContext->useRequestUrl().emit(pContext->useEventVisitor().useName());

            //a_Add event of URL requested
            ARope rope("RequestURL[",11);
            pContext->useRequestUrl().emit(rope);
            rope.append(']');
            pContext->setExecutionState(rope);
            m_Services.useLog().add(rope, ALog::INFO);
          }
          break;

          default:
            AASSERT(this, false);   //a_Should not be here, AOSContext::Status not handled
        }

        //a_Programming error, should never be NULL here
        //a_Check above to make sure if a state was changed that it didn't fall through here by accident
        //a_After state change must continue to get the next context
        AASSERT(this, pContext);

        //a_Create/Use session cookie/data
        AString strSessionId;
        pContext->useRequestCookies().getValue(ASW("AOSSession", 10), strSessionId);
        if (!strSessionId.isEmpty() && m_Services.useSessionManager().exists(strSessionId))
        {
          //a_Fetch session
          pContext->setExecutionState(AString("Using existing session ",23)+strSessionId);
          AOSSessionData *pSessionData = m_Services.useSessionManager().getSessionData(strSessionId);
          pContext->setSessionObject(pSessionData);
        }
        else
        {
          //a_Create session
          if (!strSessionId.isEmpty())
          {
            pContext->setExecutionState(ARope("Creating new session, existing session not found: ",50)+strSessionId);
            strSessionId.clear();
          }
          
          //a_Fetch session (creating a new one)
          //a_This call will populate steSessionId and return the new session data object
          pContext->setSessionObject(m_Services.useSessionManager().createNewSessionData(strSessionId));

          //a_Add cookie for this session
          ACookie& cookie = pContext->useResponseCookies().addCookie(ASW("AOSSession", 10), strSessionId);
          cookie.setMaxAge(3600);
          cookie.setPath(ASW("/",1));
          pContext->setExecutionState(ASW("Created new session ",20)+strSessionId);
        }

        //
        //a_Process directory config
        //
        if (pContext->getDirConfig())
          m_Services.useModuleExecutor().execute(*pContext, pContext->getDirConfig()->getModules());
        else
          pContext->useEventVisitor().set(ASW("No directory config for this path, skipping.",44));

        //a_If error is logged stop and go to error handler
        if (pContext->useEventVisitor().getErrorCount() > 0)
        {
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          continue;
        }

        //
        //a_Handle redirection due to directory config
        //
        if (pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_REDIRECTING))
        {
          //a_The writing of the output header
          pContext->useResponseHeader().emit(pContext->useSocket());
          pContext->useContextFlags().setBit(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT);

          //a_No error and pipelining, we handled request, do not continue
          if (!pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING))
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);

          continue;
        }

        //
        //a_Command processing
        //
        const AOSCommand *pCommand = pContext->getCommand();
        if (pCommand && pCommand->isEnabled())
        {
          //
          //a_Send context to execution queue
          //
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_EXECUTE, &pContext);
          continue;
        }
        else
        {
          //a_Static file detected
          //a_Add Date: to the response
          {
            ATime timeNow;
            AString str;
            timeNow.emitRFCtime(str);
            pContext->useResponseHeader().setPair(AHTTPHeader::HT_GEN_Date, str);
          }

          //
          //a_Serve page (static)
          //
          bool processSuccess = _processStaticPage(pContext);
          if (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_SOCKET_ERROR))
          {
            //a_Socket error occured, just terminate and keep processing
            pContext->setExecutionState(ASW("AOSContextQueue_PreExecutor: Client socket error detected, terminating request",78));
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
          }
          else
          {
            //a_If an error occured, send context to error handler and continue
            if (!processSuccess)
            {
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
            }
            else if (!pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING))
            {
              //a_No error, but not pipelining, terminate and continue
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
            }
          }

          //a_Handle next request in the pipeline
          continue;
        }
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
      pContext->setExecutionState(ASW("Unknown exception caught in AOSContextQueue_PreExecutor::threadproc",67), true);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
      break;
    }
  }

  thread.setRunning(false);
  return 0;
}

bool AOSContextQueue_PreExecutor::_processStaticPage(AOSContext *pContext)
{
  AASSERT(this, pContext);
  
  pContext->setExecutionState(ASW("Serving static content",22));
  AFilename httpFilename(m_Services.useConfiguration().getAosBaseStaticDirectory());

  static const AString ROOT_PATH("/", 1);
  AString filepart(pContext->useRequestUrl().getPathAndFilename());
  if (filepart.isEmpty() 
      || filepart.equals(ROOT_PATH)
      || pContext->useRequestUrl().getFilename().isEmpty()
  )
  {
    httpFilename.join(filepart, true);
    httpFilename.useFilename().assign(m_Services.useConfiguration().getAosDefaultFilename());
  }
  else
  {
    httpFilename.join(pContext->useRequestUrl().getPathAndFilename(), false);
  }
  
  if (!httpFilename.isValid())
  {
    //a_Invalid filename
    pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_400_Bad_Request);
    return false;
  }
  
  int dumpContext = pContext->getDumpContextLevel();
  int gzipLevel = pContext->calculateGZipLevel();

  AAutoPtr<AFile> pFile;
  size_t contentLength = AConstant::npos;
  const ATime& ifModifiedSince = pContext->useRequestHeader().getIfModifiedSince();
  ATime modified;
  switch (m_Services.useCacheManager().getStaticFile(*pContext, httpFilename, pFile, modified, ifModifiedSince))
  {
    case ACache_FileSystem::NOT_FOUND:
      //a_Handle file not found
      pContext->setExecutionState(ARope("File not found (HTTP-404 static): ",34)+httpFilename);
      pContext->useServices().useLog().add(ASW("File not found (HTTP-404 static): ",34), httpFilename, ALog::INFO);
      
      //a_Set response status and return with failed (display error template)
      pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_404_Not_Found);
    return false;

    case ACache_FileSystem::FOUND_NOT_MODIFIED:
      pContext->setExecutionState(ARope("File not modified (HTTP-304): ",30)+httpFilename);
      
      //a_Set status 304 and return true (no need to display error template)
      pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_304_Not_Modified);
      pContext->writeResponseHeader();
    return true;

    case ACache_FileSystem::FOUND_NOT_CACHED:
    case ACache_FileSystem::FOUND:
      if (!dumpContext && !gzipLevel)
      {
        //a_Context not dumped and no compression is needed
        AASSERT(pContext, !pFile.isNull());
        
        //a_Set modified date
        pContext->useResponseHeader().setLastModified(modified);
        pContext->useResponseHeader().setPair(AHTTPResponseHeader::HT_ENT_Content_Length, AString::fromS8(AFileSystem::length(httpFilename)));
        
        AString ext;
        httpFilename.emitExtension(ext);
        pContext->setResponseMimeTypeFromExtension(ext);
        pContext->writeResponseHeader();

        //a_Stream content
        pContext->setExecutionState(ARope("Streaming file: ",16)+httpFilename);
        contentLength = pFile->flush(pContext->useSocket());
        pContext->setExecutionState(ARope("Streamed bytes: ",16)+AString::fromSize_t(contentLength));
        pContext->useEventVisitor().reset();
        pContext->useContextFlags().setBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT);
        return true;
      }
      else
      {
        //a_Buffer the file
        AASSERT(pContext, !pFile.isNull());

        //a_Set modified date
        pContext->useResponseHeader().setLastModified(modified);

        //a_Buffer the file, Content-Length set in the write header
        pContext->setExecutionState(ARope("Sending file: ",14)+httpFilename);
        pFile->emit(pContext->useOutputBuffer());
        contentLength = pContext->getOutputBufferSize();
        pContext->setExecutionState(ARope("Sent bytes: ",12)+AString::fromSize_t(contentLength));
        pContext->useEventVisitor().reset();
      }
    break;

    default:
      AASSERT(pContext, false);  //a_Unknown return type?
  }

  AString str;
  if (dumpContext > 0)
  {
    //a_Dump context as XML instead of usual output
    pContext->useModel().addElement(ASW("/context/dump",13), *pContext);
    pContext->useModel().addElement(ASW("/context/buffer",15), pContext->useOutputBuffer(), AXmlElement::ENC_CDATAHEXDUMP);

    //a_Clear the output buffer and force type for be XML, code below will emit the doc into buffer
    pContext->clearOutputBuffer();
    contentLength = AConstant::npos;
  }

  //a_If output buffer is empty then emit output XML document into it unless output was read from file then contentLength != -1
  //a_This allows override of XML emit by manually adding data to the output buffer in output generator
  if (
    AConstant::npos == contentLength
    && pContext->isOutputBufferEmpty() 
    && !pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT)
  )
  {
    pContext->useModelXmlDocument().emit(pContext->useOutputBuffer());
    pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/xml", 8));
  }
  
  try
  {
    pContext->writeOutputBuffer();
  }
  catch(ASocketException& ex)
  {
    pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
    pContext->useEventVisitor().set(ex, false);
    return false;
  }

  pContext->useEventVisitor().reset();
  return true;
}
