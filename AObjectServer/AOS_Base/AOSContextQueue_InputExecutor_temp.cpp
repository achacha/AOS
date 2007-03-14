#include "pchAOS_Base.hpp"
#include "AOSContextQueue_InputExecutor.hpp"
#include "AOSContext.hpp"
#include "AOSOutputContext.hpp"
#include "AOSCommand.hpp"
#include "AOSServices.hpp"
#include "AOSInputExecutor.hpp"
#include "AOSModuleExecutor.hpp"
#include "AOSOutputExecutor.hpp"
#include "AFile_Physical.hpp"
#include "ADatabasePool.hpp"
#include "AStackWalker.hpp"
#include "AZlib.hpp"
#include "AFileSystem.hpp"

#define EMPTY_QUEUE_READ_DELAY 30

const AString& AOSContextQueue_InputExecutor::getClass() const
{
  static const AString CLASS("AOSContextQueue_InputExecutor");
  return CLASS;
}

void AOSContextQueue_InputExecutor::addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request) const
{
  AOSAdminInterface::addAdminXml(eBase, request);
  
  addPropertyWithAction(
    eBase,
    ASW("thread_count",12),
    AString::fromSize_t(m_executorCount),
    ASW("Update",6),
    ASW("Set thread count",16),
    ASW("value",5)
  );

  addProperty(
    eBase,
    ASW("dispatcher_thread_running",25),
    AString::fromBool(m_threadDispatcher.isRunning())
  );
  
  addProperty(
    eBase,
    ASW("dispatcher_thread_run",21),
    AString::fromBool(m_threadDispatcher.isRun())
  );

  //a_Status for every thread
  int i = 0;
  EXECUTORS::const_iterator cit = m_Executors.begin();
  while (cit != m_Executors.end())
  {
    AThread *pthread = dynamic_cast<AThread *>(*cit);
    AXmlElement& eThread = eBase.addElement(ASW("object",6))
      .addAttribute(ASW("name",4), ASW("AThread",7))
      .addAttribute(ASW("index",5), AString::fromInt(i))
      .addAttribute(ASW("oid",3), AString::fromU4(pthread->getId())
    );
    AXmlElement& eRunProperty = addPropertyWithAction(
      eThread,
      ASW("isRun",5),
      AString::fromBool(pthread->isRun()),
      ASW("Kill",4),
      ASW("Set thread for termination",26)
    );
    addActionToProperty(
      eRunProperty,
      ASW("WalkStack",9),
      ASW("Walk thread call stack",22)
    );
    addProperty(
      eThread,
      ASW("isRunning",9),
      AString::fromBool(pthread->isRunning())
    );
    addProperty(
      eThread,
      ASW("isIdle",6),
      AString::fromBool(pthread->getBitArray().isBit(AOSContextQueue_InputExecutor::BITNUM_IDLE))
    );
    addProperty(
      eThread,
      ASW("state",5),
      pthread->getExecutionState()
    );
    if (pthread->getBitArray().isBit(AOSContextQueue_InputExecutor::BITNUM_WALKSTACK))
    {
      AStackWalker sw(AStackWalker::SWO_SET_STACKONLY);
      sw.ProcessCallstack(pthread->getHandle());
      addProperty(
        eThread,
        ASW("callstack",9),
        sw,
        AXmlData::CDataSafe
      );
      pthread->useBitArray().clearBit(AOSContextQueue_InputExecutor::BITNUM_WALKSTACK);
    }

    ++cit;
    ++i;
  }
}

void AOSContextQueue_InputExecutor::processAdminAction(AXmlElement& base, const AHTTPRequestHeader& request)
{
  AString propertyName, actionName;
  if (!request.getUrl().getQueryString().get(PROPERTY, propertyName))
  {
    addError(base, "'property' input not found");
    return;
  }
  if (!request.getUrl().getQueryString().get(ACTION, actionName))
  {
    addError(base, "'action' input not found");
    return;
  }

  if (propertyName.equals("AOSContextQueue_InputExecutor.thread_count"))
  {
    AString value;
    if (request.getUrl().getQueryString().get(ASW("value",5), value))
    {
      int count = value.toInt();
      if (count > 0 && count < 100)
      {
        setExecutorCount(count);
      }
      else
        addError(base, "Thread count not [1,99]");
    }
    else
      addError(base, "'value' parameter not found for action 'thread_count'");
  }
  else if (0 == propertyName.find("AOSContextQueue_InputExecutor.AThread["))
  {
    //a_Individual thread control
    AString strId;
    if (request.getUrl().getQueryString().get(ASW("oid",3), strId))
    {
      u4 thread_id = strId.toU4();
      EXECUTORS::const_iterator cit = m_Executors.begin();
      while (cit != m_Executors.end())
      {
        AThread *pthread = dynamic_cast<AThread *>(*cit);
        if (pthread->getId() == thread_id)
        {
          if (0 == actionName.find("Kill"))
          {
            pthread->setRun(false);
            addMessage(base, ARope("Thread[")+strId+"] marked for death.");
            break;
          }
          else if (0 == actionName.find("WalkStack"))
          {
            //a_Set bit and after this call addAdminXml is called and will walk the stack
            pthread->useBitArray().setBit(AOSContextQueue_InputExecutor::BITNUM_WALKSTACK);
            break;
          }
          else
          {
            addError(base, ARope("Unknown action: ")+actionName);
          }
        }
        ++cit;
      }
    }
    else
      addError(base, ARope("Thread id ")+strId+" does not exist");
    
  }
  else
    addError(base, ARope("Do not know how to modify: ")+propertyName);
}

AOSContextQueue_InputExecutor::AOSContextQueue_InputExecutor(
  AOSServices& services,
  AOSInputExecutor& inputExecutor,
  AOSModuleExecutor& moduleExecutor,
  AOSOutputExecutor& outputExecutor,
  AOSContextQueueSet *pForward,      // = NULL 
  AOSContextQueueSet *pBack          // = NULL
) :
  AOSContextQueueInterface(pForward, pBack),
  m_Services(services),
  m_InputExecutor(inputExecutor),
  m_ModuleExecutor(moduleExecutor),
  m_OutputExecutor(outputExecutor)
{
  m_executorCount = services.useConfiguration().getExecutorThreadCount();
  AASSERT(this, m_executorCount > 0);

  registerAdminObject(m_Services.useAdminRegistry());
}

void AOSContextQueue_InputExecutor::setExecutorCount(u1 count /* = 3 */)
{
  m_executorCount = count;
}

u4 AOSContextQueue_InputExecutor::_threadprocDispatcher(AThread& thread)
{
  AOSContextQueue_InputExecutor *pThis = static_cast<AOSContextQueue_InputExecutor *>(thread.getThis());
  AASSERT(NULL, pThis);

  const u4 THREAD_TIMEOUT = pThis->m_Services.useConfiguration().getExecutorThreadTimeout();
  const u4 SHORT_SLEEP = 1000;
  const u4 LONG_SLEEP = 6000;
  thread.setRunning(true);
  try
  {
    int shutdownTries = 6;
    while(thread.isRun() || pThis->m_Executors.size() > 0)
    {
      int diff = int(pThis->m_executorCount) - int(pThis->m_Executors.size());
      if (diff > 0)
      {
        //a_Create more
        while (diff > 0)
        {
          AThread *pthread = new AThread(_threadprocExecutor, true, pThis);
          pthread->useBitArray().setBit(AOSContextQueue_InputExecutor::BITNUM_IDLE, true);         //a_Starts off as idle
          pThis->m_Executors.push_back(pthread);
          pThis->m_Services.useLog().add(ASWNL("DISPATCH: Adding thread id"), AString::fromInt(pthread->getId()), ALog::MESSAGE);
          --diff;
        }
      }
      else if (diff < 0)
      {
        //a_Flag some inactive and remove later
        EXECUTORS::iterator it = pThis->m_Executors.begin();
        while (it != pThis->m_Executors.end() && diff< 0)
        {
          if ((*it)->isRun())
          {
            (*it)->setRun(false);
            if (!(*it)->getUserTimer().isRunning())
              (*it)->startUserTimer(THREAD_TIMEOUT);
            ++diff;
          }
          ++it;
        }
      }

      //a_Snooze and wait for some threads to stop running then remove
      AThread::sleep(SHORT_SLEEP);
      
      //a_Thread removal
      EXECUTORS::iterator it = pThis->m_Executors.begin();
      while (it != pThis->m_Executors.end())
      {
        if ((*it)->isRunning())
        {
          //a_Only forcefully terminate threads that have timed out, if they have not, left them exit gracefully until timeout is met
          if ((*it)->isUserTimerTimedOut())
          {
            //a_Terminate threads that should not run but are still running and timed out
            EXECUTORS::iterator killIt = it;
            ++it;
            AThread *pthread = (*killIt);
            pThis->m_Executors.erase(killIt);
            pThis->m_Services.useLog().add(ASWNL("DISPATCH: Terminating and removing timed out thread id"), AString::fromInt(pthread->getId()), ALog::MESSAGE);
            pthread->terminate(-1);
            delete pthread;                                                         
          }
          else
            ++it;  //a_Active thread
        }
        else
        {
          //a_Remove threads that are not running anymore (threads exited nicely)
          EXECUTORS::iterator killIt = it;
          ++it;
          AThread *pthread = (*killIt);
          pThis->m_Executors.erase(killIt);
          pThis->m_Services.useLog().add(ASWNL("DISPATCH: Removing thread id"), AString::fromInt(pthread->getId()), ALog::MESSAGE);
          delete pthread;                                                         
        }
      }      

      //a_Sleep for a bit if everything seems ok
      if (thread.isRun() && pThis->m_Executors.size() == pThis->m_executorCount)
        AThread::sleep(LONG_SLEEP);

      if (!thread.isRun() && pThis->m_Executors.size() > 0)
      {
        pThis->m_Services.useLog().add(ASWNL("DISPATCH: Request to shutdown received."), ARope("Threads left to close=")+AString::fromSize_t(pThis->m_Executors.size()), ALog::MESSAGE);
        pThis->m_executorCount = 0;  //a_Thread has been asked to stop, set count to 0
        --shutdownTries;
        if (shutdownTries <= 0)
        {
          pThis->m_Services.useLog().add(ASWNL("DISPATCH: Bringing out the sledgehammer."), ALog::WARNING);
          EXECUTORS::iterator it2 = pThis->m_Executors.begin();
          while (it2 != pThis->m_Executors.end())
          {
            AThread *pthread = (*it2);
            pthread->terminate(0);
            pThis->m_Services.useLog().add(ASWNL("DISPATCH: Terminating thread id"), AString::fromInt(pthread->getId()), ALog::WARNING);
          }
        }
      }
    }
  }
  catch(AException& ex)
  {
    pThis->m_Services.useLog().addException(ex);
    thread.setRunning(false);
    throw;
  }
  catch(...)
  {
    pThis->m_Services.useLog().add(ASWNL("Unknown exception caught in AOSContextQueue_InputExecutor::threadprocDispatcher"), ALog::EXCEPTION);
    thread.setRunning(false);
    throw;
  }
  thread.setRunning(false);
  
  pThis->m_Services.useLog().add(ASWNL("DISPATCH: Shutdown processed."), ALog::MESSAGE);
  return 0;
}

u4 AOSContextQueue_InputExecutor::_threadprocExecutor(AThread& thread)
{
  thread.setRunning(true);

  AOSContextQueue_InputExecutor *pThis = static_cast<AOSContextQueue_InputExecutor *>(thread.getParameter());
  static const AString s_logPrefix("URL");

  AOSContext *pContext = NULL;
  try
  {
//    AOSContext context(pThis->m_Services);
    AString str(1024, 256);
    thread.useExecutionState().assign(ASW("Waiting for request",19));
    while(thread.isRun())
    {
      if (pContext = pThis->_nextContext()) 
      {
        if (!ADebugDumpable::isPointerValid(pContext))
        {
          AString error("AOSContext pointer is invalid: ");
          error.append(AString::fromPointer(pContext));
          AASSERT_EX(NULL, false, error.c_str());
          continue;
        }

        //a_Associate context with request and thread
//        context.setThread(&thread);
//        context.setContextThreadIdle(false);  //a_Thread is marked as busy
//        context.setExecutionState(ASW("Setting up request",18));
//        context.reset(pRequest);
        
        pContext->setExecutionState(ASW("Processing request",18));
        bool processed = pThis->_processRequest(*pContext);
        double dElapsed = pRequest->getTimer().getInterval();
        if (!processed)
        {
          pThis->m_Services.useLog().add(s_logPrefix, ASWNL("Failed to process request, exiting this thread"), str, AString::fromDouble(dElapsed), ALog::FAILURE);
          try 
          {
            //a_Since processing failed, these may fail also
            pContext->useAFile().close();
            pContext->setExecutionState(ASW("Waiting for request",19));
            //pContext->setContextThreadIdle(true);   //a_Thread is now marked idle
            //pContext->setThread(NULL);              //a_Disassociate from thread
            AContext::deallocate(pContext);
          }
          catch(AException& ex)
          {
            pThis->m_Services.useLog().add(s_logPrefix, ASWNL("Exception during failure cleanup"), ex, ALog::WARNING);
          }
          catch(...)
          {
            pThis->m_Services.useLog().add(s_logPrefix, ASWNL("Unknown exception during failure cleanup"), ALog::WARNING);
          }
          continue;
        }
        else
        {
          pContext->setExecutionState(ASW("Finished processing request",27));
          str.clear();
          pContext->useRequestHeader().useUrl().emit(str);

          //a_Log request was successfully processed
          AFile_Socket *pSocket = dynamic_cast<AFile_Socket *>(&pRequest->useAFile());
          if (pSocket)
          {
            ARope rope(pSocket->getAddress());
            pThis->m_Services.useLog().add(s_logPrefix, rope, str, AString::fromDouble(dElapsed), ALog::INFO);
          }
          else
          {
            pThis->m_Services.useLog().add(s_logPrefix, str, AString::fromDouble(dElapsed), ALog::INFO);
          }
        }

        //a_For now close, with keep alive and HTTP/1.1 pipelining, we may need to recycle this
        #pragma message("HTTP pipelining not yet implemented")

        pContext->useAFile().close();
        pContext->setExecutionState(ASW("Waiting for request",19));
//        pContext->setContextThreadIdle(true);  //a_Thread is now marked idle
//        pContext->setThread(NULL);             //a_Disassociate from thread
        AContext::deallocate(pContext);
      }
      else
      {
        AThread::sleep(EMPTY_QUEUE_READ_DELAY);  //a_Empty queue, avoid thrashing
      }
    }
  }
  catch(AException& e)
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer dispatcher thread caught an exception: " << e.what() << std::endl;
    pThis->m_Services.useLog().addException(e);
    thread.setRunning(false);
    thread.useExecutionState().assign(e.what());
    throw;
  }
  catch(...)
  {
    ALock lock(pThis->m_Services.useScreenSynch());
    std::cout << "AObjectServer dispatcher thread caught an unknown exception." << std::endl;
    pThis->m_Services.useLog().add(AString("Unknown exception caught in AOSContextQueue_InputExecutor::threadprocDispatcher"), ALog::EXCEPTION);
    thread.setRunning(false);
    thread.useExecutionState().assign(ASW("Unknown exception",17));
    throw;
  }

  thread.useExecutionState().assign(ASW("Stopped",7));
  thread.setRunning(false);

  return 0;
}

bool AOSContextQueue_InputExecutor::_processRequest(AOSContext& context)
{
  try
  {
    context.setExecutionState(ASWNL("Reading HTTP header"));
    context.startThreadTimer(m_Services.useConfiguration().getHeaderReadTimeout());
    if (!context.readHttpHeader())
    {
      context.clearThreadTimer();
      return false;
    }
    context.clearThreadTimer();

    //a_Check if command exists
    int dumpContext = 0;
    AString str;
    if (context.useRequestQuery().get(ASW("dumpContext", 11), str))
    {
      dumpContext = str.toInt();
    }

    context.useResponseHeader().setPair(AHTTPHeader::HT_GEN_Connection, ASW("close", 5));  //NOTE: adjust when pipelining done

    const AOSCommand *pCommand = context.getCommand();
    if (pCommand && pCommand->isEnabled())
    {
      context.setExecutionState("Executing dynamic command");
      if (pCommand->isSession())
      {
        //a_Create/Use session cookie
        AString strSessionId;
        context.useRequestCookies().getValue(ASW("AOSSession", 10), strSessionId);
        if (m_SessionManager.exists(strSessionId))
        {
          //a_Fetch session
          m_Services.useLog().add(ASW("Using existing session",22),strSessionId, ALog::INFO);
          AOSSessionData *pSessionData = m_SessionManager.getSessionData(strSessionId);
          context.setSessionObject(pSessionData);
        }
        else
        {
          //a_Create session
          if (!strSessionId.isEmpty())
          {
            m_Services.useLog().add(ASW("Existing session not found",26), strSessionId, ALog::INFO);
          }

          static AString localHostHash(AString::fromSize_t(ASocketLibrary::getLocalHostName().getHash()));
          strSessionId.clear();
          ATextGenerator::generateRandomAlphanum(strSessionId, 13);
          strSessionId.append(AString::fromSize_t(ATime::getTickCount()));
          strSessionId.append(localHostHash);

          context.setSessionObject(m_SessionManager.getSessionData(strSessionId));

          m_Services.useLog().add(ASW("Creating new session",20), strSessionId, ALog::INFO);

          //a_Add cookie for this session
          ACookie& cookie = context.useResponseCookies().addCookie(ASW("AOSSession", 10), strSessionId);
          cookie.setMaxAge(3600);
          cookie.setPath(ASW("/",1));
        }
      }

      //a_Execute input processor
      //a_Processing of content-type is done here
      context.setExecutionState("Input processor");
      m_InputExecutor.execute(context);

      //a_Add input to context
//      if (!context.isAjax())
//        context.useOutputRootXmlElement().addElement(ASW("/input/AUrl", 11), context.useRequestHeader().useUrl());

      //a_Execute modules and prepare transformation data
      //a_AContext contains all the information needed to execute all required modules
      //a_Each module will manipulate the context objects (AOSContext::useObjects()) and build a data tree
      context.setExecutionState("Module executor");
      m_ModuleExecutor.execute(context);

#ifdef __DEBUG_DUMP__
      //a_Dump context contents if requested
      if (1 < dumpContext)
      {
        AString strX(4096, 1024);
        context.debugDumpToAOutputBuffer(strX);
        context.useOutputRootXmlElement().addElement(ASW("/context/debugDump", 18), strX, AXmlData::CDataSafe);
      }
#endif

      //a_Publish errors before output stage (to display by output stage if needed)
      if (context.useEventVisitor().didErrorOccur())
      {
        //a_Publish context visitor
        context.useOutputRootXmlElement().addElement(ASW("/error/EventVisitor", 19), context.useEventVisitor(), AXmlData::CDataSafe);
      }

      //a_Transform data in AOSContext::useObjects() to output depending on the format specified in AOSContext
      //a_Adds /root/total_time inside
      context.setExecutionState("Output generator");
      m_OutputExecutor.execute(context);
    }
    else
    {
      //a_Static execution
      context.setExecutionState(ARope("Static execution: ")+context.useRequestUrl());
      static const AString AOS_EXTENSION("aos", 3);
      if (context.useRequestUrl().isExtension(AOS_EXTENSION))
      {
        //a_Execute AOS executable as CGI (TODO if ever, may not need this)
        context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_404_Not_Found);
        context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/html", 9));
        context.useOutputBuffer().append("<html><head><title>");
        context.useOutputBuffer().append("AOS: AOS executables not implemented");
        context.useOutputBuffer().append("</title></head><body><b><font color='red'>Error 404: AOS executable not implemented: </b></font><big>");
        context.useRequestUrl().emit(context.useOutputBuffer());
        context.useOutputBuffer().append("</big></body></html>");
      }
      else
      {
        //a_Serve page
        AFilename httpFilename(m_Services.useConfiguration().getAosBaseStaticDirectory(), true);

        static const AString ROOT_PATH("/", 1);
        AString filepart(context.useRequestUrl().getPathAndFilename());
        if (filepart.isEmpty() 
            || filepart.equals(ROOT_PATH)
            || context.useRequestUrl().getFilename().isEmpty()
        )
        {
          httpFilename.join(filepart, true);
          httpFilename.useFilename().assign(m_Services.useConfiguration().getAosDefaultFilename());
        }
        else
        {
          httpFilename.join(context.useRequestUrl().getPathAndFilename());
        }

        context.setExecutionState(ARope("Static content: ")+httpFilename);
        if (AFileSystem::exists(httpFilename))
        {
          AFile_Physical localFile(httpFilename.toAString(), "rb");
          localFile.open();
          //a_Send the file
          localFile.readUntilEOF(context.useOutputBuffer());
          localFile.close();
        }
        else
        {
          //a_File not found
          context.useResponseHeader().setStatusCode(AHTTPResponseHeader::SC_404_Not_Found);
          context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/html", 9));
          context.useOutputBuffer().append("<html><head><title>");
          context.useOutputBuffer().append("AOS: File not found");
          context.useOutputBuffer().append("</title></head><body><b><font color='red'>Error 404: File not found: </b></font><big>");
          context.useRequestUrl().emit(context.useOutputBuffer());
          context.useOutputBuffer().append("</big></body></html>");
          context.useServices().useLog().add(ASWNL("File not found (HTTP static): "), httpFilename, ALog::INFO);
        }
      }
    }

    if (dumpContext > 0)
    {
      //a_Dump context as XML instead of usual output
      context.useOutputRootXmlElement().addElement(ASW("/context/dump",13), context);
      context.useOutputRootXmlElement().addElement(ASW("/context/buffer",15), context.useOutputBuffer(), AXmlData::CDataHexDump);

      //a_Clear the output buffer and force type for be XML, code below will emit the doc into buffer
      context.useOutputBuffer().clear();
      context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Type, ASW("text/xml", 8));
    }

    //a_If output buffer is empty then emit output XML document into it
    //a_This allows override of XML emit by manually adding data to the output buffer in output generator
    if (context.useOutputBuffer().isEmpty())
    {
      context.useOutputXmlDocument().emit(context.useOutputBuffer());
    }
    
    //a_Add some common response header pairs
    {
      ATime timeNow;
      AString str;
      timeNow.emitRFCtime(str);
      context.useResponseHeader().setPair(AHTTPHeader::HT_GEN_Date, str);
    }

    AString strDeflateLevel;
    context.useRequestHeader().find(AHTTPHeader::HT_REQ_Accept_Encoding, str);
    if ( 
         AConstant::npos != str.findNoCase("deflate") 
         && context.useRequestQuery().get(ASW("gzip",4), strDeflateLevel)
       )
    {
      context.setExecutionState("Compressing and sending result");
      AString compressed;
      int gzipLevel = strDeflateLevel.toInt();
      if (gzipLevel < 1 || gzipLevel > 9)
        AZlib::deflate(context.useOutputBuffer(), compressed);
      else
        AZlib::deflate(context.useOutputBuffer(), compressed, gzipLevel);

//      context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Encoding, ASW("deflate",7));
      context.useResponseHeader().setPair(AHTTPHeader::HT_GEN_Transfer_Encoding, ASW("chunked",7));
      context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Encoding, ASW("gzip",4));
      context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(compressed.getSize()));
//      context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(context.useOutputBuffer().getSize()));

      //a_The writing of the output
      context.useResponseHeader().emit(context.useAFile());
      context.useAFile().write(compressed);
    }
    else
    {
      context.setExecutionState("Sending result");
      context.useResponseHeader().setPair(AHTTPHeader::HT_ENT_Content_Length, AString::fromSize_t(context.useOutputBuffer().getSize()));

      //a_The writing of the output
      context.useResponseHeader().emit(context.useAFile());
      context.useAFile().write(context.useOutputBuffer());
    }
  }
  catch(AException& ex)
  {
    context.addError("AOSContextQueue_InputExecutor::_processRequest", ex);
    context.setExecutionState(ASWNL("Request processing: ")+ex.what());
    return false;
  }
  catch(...)
  {
    context.addError("AOSContextQueue_InputExecutor::_processRequest", "Unknown exception");
    context.setExecutionState(ASWNL("Request processing: Unknown exception"));
    return false;
  }

  return true;
}

void AOSContextQueue_InputExecutor::stopDispatching()
{
  m_threadDispatcher.setRun(false);
}

bool AOSContextQueue_InputExecutor::isRunning() const
{
  return m_threadDispatcher.isRunning();
}

void AOSContextQueue_InputExecutor::add(AOSContext *pRequest)
{
#pragma message("TODO: AOSContextQueue_InputExecutor")
}

AOSRequest *AOSContextQueue_InputExecutor::_nextRequest()
{
  ATHROW(this, AException::ProgrammingError);
  return NULL;
}

AOSContext *AOSContextQueue_InputExecutor::_nextContext()
{
#pragma message("TODO: AOSContextQueue_InputExecutor")
  return NULL;
}
