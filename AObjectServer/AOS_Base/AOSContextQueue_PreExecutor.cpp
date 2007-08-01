#include "pchAOS_Base.hpp"
#include "AOSContextQueue_PreExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "AOSCommand.hpp"
#include "AFileSystem.hpp"
#include "AFile_Physical.hpp"
#include "AZlib.hpp"
#include "AOSConfiguration.hpp"
#include "ATemplate.hpp"

const AString& AOSContextQueue_PreExecutor::getClass() const
{
  static const AString CLASS("AOSContextQueue_PreExecutor");
  return CLASS;
}

void AOSContextQueue_PreExecutor::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  BASECLASS_AOSContextQueue_PreExecutor::addAdminXml(eBase, request);
}

void AOSContextQueue_PreExecutor::processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  BASECLASS_AOSContextQueue_PreExecutor::processAdminAction(eBase, request);
}

AOSContextQueue_PreExecutor::AOSContextQueue_PreExecutor(
  AOSServices& services,
  size_t threadCount,              // = 16
  size_t queueCount,               // = 4
  AOSContextQueueInterface *pYes,  // = NULL 
  AOSContextQueueInterface *pNo    // = NULL
) :
  BASECLASS_AOSContextQueue_PreExecutor(services, threadCount, queueCount, pYes, pNo)
{
  useThreadPool().setParameter(this);
  registerAdminObject(m_Services.useAdminRegistry());
}

AOSContextQueue_PreExecutor::~AOSContextQueue_PreExecutor()
{
}

u4 AOSContextQueue_PreExecutor::_threadproc(AThread& thread)
{
  AOSContextQueue_PreExecutor *pThis = static_cast<AOSContextQueue_PreExecutor *>(thread.getParameter());
  AASSERT(NULL, pThis);
  AOSContext *pContext = NULL;

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
#ifdef __DEBUG_DUMP__
        if (!ADebugDumpable::isPointerValid(pContext))
        {
          AString error("AOSContextQueue_PreExecutor: AOSContext pointer is invalid: ");
          error.append(AString::fromPointer(pContext));
          AASSERT_EX(NULL, false, error.c_str());
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
          case AOSContext::STATUS_HTTP_INCOMPLETE_METHOD:
          case AOSContext::STATUS_HTTP_INCOMPLETE_LINEZERO:
          case AOSContext::STATUS_HTTP_INCOMPLETE_CRLFCRLF:
            //a_Go into waiting queue
            pContext->setExecutionState(ASW("AOSContextQueue_PreExecutor: HTTP header not complete, going into waiting queue",79));
            pThis->_goNo(pContext);
            pContext = NULL;
          continue;

          case AOSContext::STATUS_HTTP_INVALID_AFTER_METHOD_CHAR:
          case AOSContext::STATUS_HTTP_INVALID_FIRST_CHAR:
          {
            
            ARope rope("AOSContextQueue_PreExecutor(Failed AOSContext::init)",52);
  #ifdef __DEBUG_DUMP__
            rope.append(AString::sstr_EOL);
            pContext->debugDumpToAOutputBuffer(rope);
  #endif
            pContext->useEventVisitor().set(rope, true);

            //a_If already waited in the is available queue terminate, else have it a go
            if (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_AVAILABLE_SELECTED))
              pThis->_goTerminate(pContext);
            else
              pThis->_goNo(pContext);

            pContext = NULL;
          }
          continue;

          case AOSContext::STATUS_HTTP_SOCKET_CLOSED:
          {
            //a_Socket was closed, terminate
            pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
            pThis->_goTerminate(pContext);
            pContext = NULL;
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

        const AOSCommand *pCommand = pContext->getCommand();
        if (pCommand && pCommand->isEnabled())
        {
          //a_AOS command (dynamic)
          if (pCommand->isSession())
          {
            //a_Create/Use session cookie
            AString strSessionId;
            pContext->useRequestCookies().getValue(ASW("AOSSession", 10), strSessionId);
            if (m_Services.useSessionManager().exists(strSessionId))
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
                pContext->setExecutionState(AString("Existing session not found ",27)+strSessionId);
              }

              pContext->setExecutionState(ASW("Creating new session",20));
              
              AString localHostHash(AString::fromSize_t(ASocketLibrary::getLocalHostName().getHash()));
              strSessionId.clear();
              ATextGenerator::generateRandomAlphanum(strSessionId, 13);
              strSessionId.append(AString::fromSize_t(ATime::getTickCount()));
              strSessionId.append(localHostHash);

              pContext->setSessionObject(m_Services.useSessionManager().getSessionData(strSessionId));

              //a_Add cookie for this session
              ACookie& cookie = pContext->useResponseCookies().addCookie(ASW("AOSSession", 10), strSessionId);
              cookie.setMaxAge(3600);
              cookie.setPath(ASW("/",1));
              pContext->setExecutionState(ASW("Created new session ",20)+strSessionId);
            }
          }
          
          //a_Go to next stage
          pThis->_goYes(pContext);
          pContext = NULL;
        }
        else
        {
          //a_Serve page (static)
          bool processSuccess = _processStaticPage(pContext);
          
          if (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_SOCKET_ERROR))
          {
            //a_Socket error occured, just terminate and keep processing
            pThis->_goTerminate(pContext);
            pContext = NULL;
          }
          else
          {
            //a_If an error occured, send context to error handler and continue
            if (!processSuccess)
            {
              pThis->_goError(pContext);
              pContext = NULL;
            }
            else if (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING))
            {
              //a_No error and pipelining, we handled request, do not continue
              pThis->_goNo(pContext);
              pContext = NULL;
            }
            else
            {
              //a_No error, but not pipelining, terminate and continue
              pThis->_goTerminate(pContext);
              pContext = NULL;
            }
          }

          continue;
        }
      }
      AThread::sleep(3);  //a_Empty queue, avoid thrashing
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
      _goError(pContext);
      pContext = NULL;
    }
    catch(...)
    {
      pContext->setExecutionState(ASW("Unknown exception caught in AOSContextQueue_PreExecutor::threadproc",67), true);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      pThis->_goError(pContext);
      pContext = NULL;
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
    httpFilename.join(pContext->useRequestUrl().getPathAndFilename());
  }
  
  AAutoPtr<AFile> pFile;
  size_t contentLenth = AConstant::npos;
  if (m_Services.useCacheManager().getStaticFile(*pContext, httpFilename, pFile))
  {
    //a_Send the file
    AASSERT(pContext, !pFile.isNull());
    pFile->emit(pContext->useOutputBuffer());
    pContext->useEventVisitor().reset();
    contentLenth = pContext->useOutputBuffer().getSize();
  }
  else
  {
    //a_Handle file not found
    pContext->setExecutionState(ARope("File not found (HTTP static): ",30)+httpFilename);
    pContext->useServices().useLog().add(ASWNL("File not found (HTTP static): "), httpFilename, ALog::INFO);
    
    //a_Set response status and return with failed
    pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_404_Not_Found);
    return false;
  }

  int dumpContext = 0;
  AString str;
  if (pContext->useRequestParameterPairs().get(ASW("dumpContext", 11), str))
  {
    dumpContext = str.toInt();
  }
  if (dumpContext > 0)
  {
    //a_Dump context as XML instead of usual output
    pContext->useOutputRootXmlElement().addElement(ASW("/context/dump",13), *pContext);
    pContext->useOutputRootXmlElement().addElement(ASW("/context/buffer",15), pContext->useOutputBuffer(), AXmlData::CDataHexDump);

    //a_Clear the output buffer and force type for be XML, code below will emit the doc into buffer
    pContext->useOutputBuffer().clear();
    pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/xml", 8));
  }

  //a_If output buffer is empty then emit output XML document into it unless output was read from file then contentLength != -1
  //a_This allows override of XML emit by manually adding data to the output buffer in output generator
  if (
    AConstant::npos == contentLenth
    && pContext->useOutputBuffer().isEmpty() 
    && !pContext->useContextFlags().isSet(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT)
  )
  {
    pContext->useOutputXmlDocument().emit(pContext->useOutputBuffer());
    pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/xml", 8));
  }
  
  //a_Add some common response header pairs
  ATime timeNow;
  str.clear();
  timeNow.emitRFCtime(str);
  pContext->useResponseHeader().setPair(AHTTPHeader::HT_GEN_Date, str);

  AString strDeflateLevel;
  str.clear();
  pContext->useRequestHeader().find(AHTTPHeader::HT_REQ_Accept_Encoding, str);
  if ( 
      AConstant::npos != str.findNoCase(ASW("deflate",7)) 
      && pContext->useRequestParameterPairs().get(ASW("deflate",7), strDeflateLevel)
  )
  {
    pContext->setExecutionState(ASW("Compressing",11));
    AString compressed;
    int gzipLevel = strDeflateLevel.toInt();
    if (gzipLevel < 1 || gzipLevel > 9)
      AZlib::deflate(pContext->useOutputBuffer(), compressed);
    else
      AZlib::deflate(pContext->useOutputBuffer(), compressed, gzipLevel);

    pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Encoding, ASW("deflate",7));
    //pContext->useResponseHeader().setPair(AHTTPHeader::HT_GEN_Transfer_Encoding, ASW("chunked",7));
    //pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Encoding, ASW("gzip",4));
    pContext->useResponseHeader().setPair(AHTTPHeader::HT_RES_Vary, ASW("Accept-Encoding",15));
    pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(compressed.getSize()));
//      pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(pContext->useOutputBuffer().getSize()));

    //a_The writing of the output
    pContext->setExecutionState(ASW("Writing compressed",18));

    pContext->useResponseHeader().emit(pContext->useSocket());
    
    try
    {
      pContext->useSocket().write(compressed);
    }
    catch(AException& ex)
    {
      pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
      m_Services.useLog().add(ex);
      return false;
    }
    catch(...)
    {
      pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
      m_Services.useLog().add(ASWNL("Unknown exception: _processStaticPage: writing compressed"), ALog::CRITICAL_ERROR);
      return false;
    }
  }
  else
  {
    pContext->setExecutionState(ASW("Writing uncompressed",20));
    pContext->useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(pContext->useOutputBuffer().getSize()));

    //a_The writing of the output
    pContext->useResponseHeader().emit(pContext->useSocket());
    try
    {
      pContext->useSocket().write(pContext->useOutputBuffer());
    }
    catch(AException& ex)
    {
      pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
      m_Services.useLog().add(ex);
      return false;
    }
    catch(...)
    {
      pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_ERROR);
      m_Services.useLog().add(ASWNL("Unknown exception: _processStaticPage: writing uncompressed"), ALog::CRITICAL_ERROR);
      return false;
    }
  }

  pContext->useEventVisitor().reset();
  return true;
}
