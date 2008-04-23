#include "pchAOS_Base.hpp"
#include "AOSContextQueue_PreExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSServices.hpp"
#include "AOSController.hpp"
#include "AOSConfiguration.hpp"
#include "ASocketException.hpp"

const AString& AOSContextQueue_PreExecutor::getClass() const
{
  static const AString CLASS("AOSContextQueue_PreExecutor");
  return CLASS;
}

void AOSContextQueue_PreExecutor::adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request)
{
  adminAddProperty(
    eBase,
    ASW("WaitForHttpDataTimeout",22),
    AString::fromInt(m_WaitForHttpDataTimeout)
  );

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

  m_WaitForHttpDataTimeout = m_Services.useConfiguration().useConfigRoot().getInt(ASW("/config/server/http/wait-for-http-data-timeout",46), 10000);

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
            if (pContext->getRequestTimer().getInterval() > pThis->m_WaitForHttpDataTimeout)
            {
              //a_Waited long enough
              pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_PreExecutor: HTTP header started but did not complete before timeout, abandoning wait",101));
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
              continue;
            }
            else
            {
              //a_No data trying to read first character, go into isAvailable
              pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_PreExecutor: No data yet for HTTP header, going into waiting queue",82));
              m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_IS_AVAILABLE, &pContext);
            }
          continue;

            //Fallthrough from above
          case AOSContext::STATUS_HTTP_INCOMPLETE_METHOD:
          case AOSContext::STATUS_HTTP_INCOMPLETE_LINEZERO:
          case AOSContext::STATUS_HTTP_INCOMPLETE_CRLFCRLF:
          {
            //a_Go into waiting queue
            if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_DEBUG))
              pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_PreExecutor: HTTP header not complete, going into waiting queue",79), AEventVisitor::EL_DEBUG);
            if (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_SOCKET_ERROR))
            {
              //a_Socket error occured, just terminate and keep processing
              pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_PreExecutor: Client socket error detected before heade read, terminating request",96));
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
            if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_INFO))
              pContext->useEventVisitor().startEvent(ASW("Writing OPTIONS response",24), AEventVisitor::EL_INFO);
            pContext->useResponseHeader().emit(pContext->useSocket());
            
            //a_Flag that all is done
            pContext->useContextFlags().setBit(AOSContext::CTXFLAG_IS_RESPONSE_HEADER_SENT, true);
            pContext->useContextFlags().setBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT, true);
            pContext->useEventVisitor().endEvent();
            
            //a_Request is done
            pContext->useEventVisitor().startEvent(pContext->useRequestHeader());
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);
          }
          continue;

          case AOSContext::STATUS_HTTP_METHOD_NOT_ALLOWED:
          {
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_405_Method_Not_Allowed);
            pContext->useEventVisitor().addEvent(pContext->useRequestHeader());
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          }
          continue;

          case AOSContext::STATUS_HTTP_VERSION_NOT_SUPPORTED:
          {
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_505_HTTP_Version_Not_Supported);
            pContext->useEventVisitor().addEvent(pContext->useRequestHeader());
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          }
          continue;

          case AOSContext::STATUS_HTTP_UNKNOWN_METHOD:
          {
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_400_Bad_Request);
            pContext->useEventVisitor().addEvent(pContext->useRequestHeader());
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          }
          continue;

          case AOSContext::STATUS_HTTP_INVALID_HEADER:
          case AOSContext::STATUS_HTTP_INVALID_REQUEST_PATH:
            pContext->useEventVisitor().startEvent(pContext->useRequestHeader(), AEventVisitor::EL_INFO);
          //a_Fallthrough
          case AOSContext::STATUS_HTTP_INVALID_FIRST_CHAR:
          case AOSContext::STATUS_HTTP_INVALID_AFTER_METHOD_CHAR:
          {
            pContext->useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_400_Bad_Request);
            pContext->useEventVisitor().endEvent();
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
          }
          continue;

          case AOSContext::STATUS_HTTP_SOCKET_CLOSED:
          {
            //a_Socket was closed, terminate
            pContext->useEventVisitor().addEvent(ASW("AOSContextQueue_PreExecutor: Socket was closed by client, terminating",69), AEventVisitor::EL_ERROR);
            pContext->useConnectionFlags().setBit(AOSContext::CONFLAG_IS_SOCKET_CLOSED);
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
            pContext->useEventVisitor().startEvent(rope);
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
          if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_INFO))
            pContext->useEventVisitor().startEvent(AString("Using existing session ",23)+strSessionId, AEventVisitor::EL_INFO);
          AOSSessionData *pSessionData = m_Services.useSessionManager().getSessionData(strSessionId);
          pContext->setSessionObject(pSessionData);
        }
        else
        {
          //a_Create session
          if (!strSessionId.isEmpty())
          {
            if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_INFO))
            {
              ARope rope("Creating new session, existing session not found: ",50);
              rope.append(strSessionId);
              pContext->useEventVisitor().startEvent(rope, AEventVisitor::EL_INFO);
            }
            strSessionId.clear();
          }
          
          //a_Fetch session (creating a new one)
          //a_This call will populate steSessionId and return the new session data object
          pContext->setSessionObject(m_Services.useSessionManager().createNewSessionData(strSessionId));

          //a_Add cookie for this session
          ACookie& cookie = pContext->useResponseCookies().addCookie(ASW("AOSSession", 10), strSessionId);
          cookie.setMaxAge(3600);
          cookie.setPath(ASW("/",1));
          if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_INFO))
          {
            AString str("Created new session ",20);
            str.append(strSessionId);
            pContext->useEventVisitor().startEvent(str, AEventVisitor::EL_INFO);
          }
        }

        //
        //a_Process directory config
        //
        if (pContext->getDirConfig())
          m_Services.useModuleExecutor().execute(*pContext, pContext->getDirConfig()->getModules());
        else
        {
          if (pContext->useEventVisitor().isLogging(AEventVisitor::EL_INFO))
            pContext->useEventVisitor().startEvent(ASW("No directory config for this path, skipping.",44), AEventVisitor::EL_INFO);
        }

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
          pContext->writeResponseHeader();
          pContext->useContextFlags().setBit(AOSContext::CTXFLAG_IS_OUTPUT_SENT);

          //a_No error and pipelining, we handled request, do not continue
          if (!pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_HTTP11_PIPELINING))
            m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_TERMINATE, &pContext);

          continue;
        }

        //
        //a_Controller processing
        //
        const AOSController *pController = pContext->getController();
        if (pController && pController->isEnabled())
        {
          //
          //a_Send context to execution queue
          //
          m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_EXECUTE, &pContext);
          continue;
        }
        else
        {
          //
          //a_Serve page (static)
          //
          bool processSuccess = pContext->processStaticPage();
          if (pContext->useConnectionFlags().isSet(AOSContext::CONFLAG_IS_SOCKET_ERROR))
          {
            //a_Socket error occured, just terminate and keep processing
            pContext->useEventVisitor().startEvent(ASW("AOSContextQueue_PreExecutor: Client socket error detected, terminating request",78));
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
      pContext->useEventVisitor().addEvent(ASW("Unknown exception caught in AOSContextQueue_PreExecutor::threadproc",67), AEventVisitor::EL_ERROR);
      m_Services.useLog().add(pContext->useEventVisitor(), ALog::FAILURE);
      m_Services.useContextManager().changeQueueState(AOSContextManager::STATE_ERROR, &pContext);
      break;
    }
  }

  thread.setRunning(false);
  return 0;
}
