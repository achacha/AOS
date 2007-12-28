
#include "pchAObjectServer.hpp"

#include "AOS.hpp"
#include "AOSAdmin.hpp"
#include "AOSServices.hpp"
#include "ASocketLibrary_SSL.hpp"
#include "ADynamicLibrary.hpp"

#include "AOSRequestListener.hpp"
#include "AOSContextQueue_IsAvailable.hpp"
#include "AOSContextQueue_PreExecutor.hpp"
#include "AOSContextQueue_Executor.hpp"
#include "AOSContextQueue_ErrorExecutor.hpp"

ADynamicLibrary dllModules;                  //a_Map of DLLs loaded
ASocketLibrary_SSL g_SecureSocketLibrary;    //a_Global init for SSL and socket library

static const AString _BUILD_INFO_(AString(AOS_SERVER_NAME)+"\tBUILD("+__TIME__+" "+__DATE__+")");

void traceMultiline(const AString& str, void *ptr)
{
  AString strLine;
  AFile_AString strfile(str);
  while(AConstant::npos != strfile.readLine(strLine))
  {
    AOS_DEBUGTRACE(strLine.c_str(), ptr);
    strLine.clear();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// AObjectServer MAIN /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
  AFILE_TRACER_DEBUG_SCOPE((AString("AObjectServer::main:",20)+ASWNL(argv[0])).c_str(), NULL);

  std::cout << _BUILD_INFO_ << std::endl;
  try
  {
    //a_Check if INI filename was provided
    AFilename basePath(ASW("./aos_root/",11), false);
    if (argc > 1)
    {
      //a_Check existance of INI file
      basePath.set(AString(argv[1]), true);
    }

    if (!AFileSystem::exists(basePath))
    {
      AString str("main: Base path does not exist: ");
      basePath.emit(str);
      AOS_DEBUGTRACE(str.c_str(), NULL);
      return -1;
    }

    //a_Initialize services
    AOSServices services(basePath);
    try
    {
      //a_Ready to start
      {
        AString str;
        ATime().emitRFCtime(str);
        services.useLog().add(ASWNL("=+=+=+= AOS Server starting at ")+str, AString(_BUILD_INFO_));
      }

      AOSAdmin admin(services);
      services.registerAdminObject(services.useAdminRegistry());

      //
      //a_Initialize the global objects
      //
      AString str;
      AString strError;
      if (services.initDatabasePool())
      {
        size_t rows = services.loadGlobalObjects(strError);
        if (AConstant::npos == rows)
        {
          AOS_DEBUGTRACE((AString("DBERROR: Global init: ")+strError).c_str(), NULL);
          return -1;
        }
        else
        {
          str.clear();
          str.assign("Database connection pool connected ");
          services.useConfiguration().useConfigRoot().emitString(AOSConfiguration::DATABASE_CONNECTIONS, str);
          str.append("x to ");
          services.useConfiguration().useConfigRoot().emitString(AOSConfiguration::DATABASE_URL, str);
          AOS_DEBUGTRACE(str.c_str(), NULL);
          str.assign("Querying for global variables, processed ");
          str.append(AString::fromSize_t(rows));
          str.append(" rows.");
          AOS_DEBUGTRACE(str.c_str(), NULL);
        }
      }
      else
      {
        AOS_DEBUGTRACE("Database was not initialized, skipping global init.", NULL);
      }

      //
      //a_Create and configure the queues
      //
      AOSContextQueueThreadPool *pQueueIsAvail = new AOSContextQueue_IsAvailable(services);
      int sleepDelay = services.useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/context-queues/is-available/sleep-delay"), DEFAULT_SLEEP_DELAY);
      if (sleepDelay > 0)
        pQueueIsAvail->setSleepDelay(sleepDelay);
      else
        AOS_DEBUGTRACE("Sleep delay for is-available/sleep-delay is invalid, using default", NULL);
      
      AOSContextQueueThreadPool *pQueueError = new AOSContextQueue_ErrorExecutor(
        services,
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/error-executor/threads", 16), 
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/error-executor/queues", 4)
      );
      sleepDelay = services.useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/context-queues/error-executor/sleep-delay"), DEFAULT_SLEEP_DELAY);
      if (sleepDelay > 0)
        pQueueError->setSleepDelay(sleepDelay);
      else
        AOS_DEBUGTRACE("Sleep delay for error-executor/sleep-delay is invalid, using default", NULL);

      AOSContextQueueThreadPool *pQueuePre = new AOSContextQueue_PreExecutor(
        services, 
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/pre-executor/threads", 16), 
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/pre-executor/queues", 4)
      );
      sleepDelay = services.useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/context-queues/pre-executor/sleep-delay"), DEFAULT_SLEEP_DELAY);
      if (sleepDelay > 0)
        pQueuePre->setSleepDelay(sleepDelay);
      else
        AOS_DEBUGTRACE("Sleep delay for pre-executor/sleep-delay is invalid, using default", NULL);

      AOSContextQueueThreadPool *pQueueExecutor = new AOSContextQueue_Executor(
        services, 
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/executor/threads", 64), 
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/executor/queues", 3)
      );
      sleepDelay = services.useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/context-queues/executor/sleep-delay"), DEFAULT_SLEEP_DELAY);
      if (sleepDelay > 0)
        pQueueExecutor->setSleepDelay(sleepDelay);
      else
        AOS_DEBUGTRACE("Sleep delay for executor/sleep-delay is invalid, using default", NULL);

      //a_Configure queue state machine
      services.useContextManager().setQueueForState(AOSContextManager::STATE_PRE_EXECUTE, pQueuePre);
      services.useContextManager().setQueueForState(AOSContextManager::STATE_EXECUTE, pQueueExecutor);
      services.useContextManager().setQueueForState(AOSContextManager::STATE_IS_AVAILABLE, pQueueIsAvail);
      services.useContextManager().setQueueForState(AOSContextManager::STATE_ERROR, pQueueError);

      //a_Associate queue for the listener
      AOSRequestListener listener(services, pQueuePre);
      
      //TODO: vvvvvvvvv     This is going away
      pQueueIsAvail->setYesContextQueue(pQueuePre);

      pQueuePre->setYesContextQueue(pQueueExecutor);
      pQueuePre->setNoContextQueue(pQueueIsAvail);
      pQueuePre->setErrorContextQueue(pQueueError);

      pQueueExecutor->setYesContextQueue(pQueueIsAvail);                 //a_Used in HTTP/1.1 pipelining
      pQueueExecutor->setErrorContextQueue(pQueueError);
      //TODO: ^^^^^^^^^     This is going away

      //
      //a_Load the processors, modules, generators dynamically from DLLs
      //
      LIST_AString listModules;
      services.useConfiguration().getDynamicModuleLibraries(listModules);

      //a_Load modules
      LIST_AString::const_iterator cit = listModules.begin();
      while(cit != listModules.end())
      {
        if (!dllModules.load(*cit))
        {
          AString strDebug("Unable to load dynamic library: ");
          strDebug.append(*cit);
          AOS_DEBUGTRACE(strDebug.c_str(), NULL);
        }

        {
          AString strDebug("=====[ BEGIN: Processing library '");
          strDebug.append(*cit);
          strDebug.append("' ]=====");
          AOS_DEBUGTRACE(strDebug.c_str(), NULL);
        }

        //a_Load the corresponding XML config for each library if exists
        services.useConfiguration().loadConfig(*cit);

        //a_Register modules
        PROC_AOS_Register *procRegister = static_cast<PROC_AOS_Register *>(dllModules.getEntryPoint(*cit, "aos_register"));
        if (procRegister)
        {
          if (
            procRegister(
              services.useInputExecutor(), 
              services.useModuleExecutor(), 
              services.useOutputExecutor(), 
              services
            )
          )
          {
            AString strDebug("  FAILED to register module: '");
            strDebug.append(*cit);
            strDebug.append('\'');
            AOS_DEBUGTRACE(strDebug.c_str(), NULL);
          }
          else
          {
            AString strDebug("  Registered: '");
            strDebug.append(*cit);
            strDebug.append('\'');
            AOS_DEBUGTRACE(strDebug.c_str(), NULL);
          }
        }
        else
        {
          AString strDebug("  Module: '");
          strDebug.append(*cit);
          strDebug.append("': unable to find proc symbol: aos_register");
          AOS_DEBUGTRACE(strDebug.c_str(), NULL);
        }

        {
          AString strDebug("=====[   END: Processing library '");
          strDebug.append(*cit);
          strDebug.append("' ]=====");
          AOS_DEBUGTRACE(strDebug.c_str(), NULL);
        }
        ++cit;
      }

      //
      //a_Start all the queues (listener is the last thing to start, see below)
      //
      pQueuePre->useThreadPool().start();
      pQueueIsAvail->useThreadPool().start();
      pQueueExecutor->useThreadPool().start();
      pQueueError->useThreadPool().start();

      //
      //a_Start listener
      //
      listener.startListening();

      //
      //a_Start admin listener
      //
      admin.startAdminListener();

      do
      {
        //a_This is the watcher for the main loop, when admin is not running the server has stopped
        AThread::sleep(5000);
      }
      while(!admin.isShutdownRequested() || admin.isRunning());
    }
    catch(AException& ex)
    {
      AString str("main: ");
      str.append(ex);
      traceMultiline(str, NULL);
      services.useLog().addException(ex);
    }
    catch(std::exception& ex)
    {
      AString str("main: ");
      str.append(ASWNL(ex.what()));
      traceMultiline(str, NULL);
      services.useLog().addException(ex);
    }
    catch(...)
    {
      AOS_DEBUGTRACE("main: Unknown exception caught", NULL);
      services.useLog().add(ASWNL("main: Unknown exception caught"), ALog::EXCEPTION);
    }
  }
  catch(AException& ex)
  {
    AString str("main: ");
    str.append(ex);
    traceMultiline(str, NULL);
  }
  catch(std::exception& ex)
  {
    AString str("main: ");
    str.append(ASWNL(ex.what()));
    traceMultiline(str, NULL);
  }
  catch(...)
  {
    AOS_DEBUGTRACE("main: Unknown exception caught", NULL);
  }
 
  return 0;
}
