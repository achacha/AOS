/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAObjectServer.hpp"

#include "AOS.hpp"
#include "AOSAdmin.hpp"
#include "AOSServices.hpp"
#include "ASocketLibrary_SSL.hpp"
#include "ADynamicLibrary.hpp"
#include "AGdLibrary.hpp"

#include "AOSRequestListener.hpp"
#include "AOSContextQueue_IsAvailable.hpp"
#include "AOSContextQueue_PreExecutor.hpp"
#include "AOSContextQueue_Executor.hpp"
#include "AOSContextQueue_ErrorExecutor.hpp"

#ifdef __WINDOWS__
#include "dbghelp.h"
#include "dbgeng.h"
#pragma comment(lib, "dbghelp")
#pragma comment(lib, "dbgeng")
#endif

ADynamicLibrary dllModules;                  //a_Map of DLLs loaded
ASocketLibrary_SSL g_SecureSocketLibrary;    //a_Global init for SSL and socket library
AGdLibrary g_GdLibrary;                      //a_Global init for gd library

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
  //a_AString available for errors so that allocation is not done during exception (in case we ran out of memory, etc)
  AString str(2048, 512);
  DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(false);

#ifdef __WINDOWS__
  LPAPI_VERSION lpVersion = ::ImagehlpApiVersion();
  if (lpVersion->MajorVersion < 4)
  {
    AOS_DEBUGTRACE("dgbhelp.dll major version < 4 (check your path to make sure the latest dbghelp.dll is picked up if removed from current directory)", NULL);
    return -1;
  }
#endif

  std::cout << AOS_Base_INFO << std::endl;
  try
  {
    //a_Check if config base path was provided
    AFilename basePath(ASW("./aos_root/",11), false);
    if (argc > 1)
    {
      //a_Check existance of config base path
      basePath.set(AString(argv[1]), true);
    }

    if (!AFileSystem::exists(basePath))
    {
      str.assign("main: Base path does not exist: ");
      basePath.emit(str);
      AOS_DEBUGTRACE(str.c_str(), NULL);
      return -1;
    }
    else
    {
      AFilename absPath;
      AFileSystem::expand(basePath, absPath);
      AString str("Base aos_root directory: ");
      absPath.emit(str);
      AOS_DEBUGTRACE(str.c_str(), NULL);
    }


    //a_Initialize services
    AOSServices services(basePath);
    try
    {
      //a_Ready to start
      {
        str.clear();
        ATime().emitRFCtime(str);
        services.useLog().add(ASWNL("=+=+=+= AOS Server starting at ")+str, AString(AOS_Base_INFO));
      }

      AOSAdmin admin(services);
      services.adminRegisterObject(services.useAdminRegistry());

      //
      //a_Initialize the global objects
      //
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
      AAutoPtr<AOSContextQueueInterface> pQueueIsAvail(new AOSContextQueue_IsAvailable(
        services,
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/is-available/queues", 2)
      ), true);
      
      AAutoPtr<AOSContextQueueThreadPool> pQueueError(new AOSContextQueue_ErrorExecutor(
        services,
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/error-executor/threads", 16), 
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/error-executor/queues", 4)
      ), true);
      int sleepDelay = services.useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/context-queues/error-executor/sleep-delay"), DEFAULT_SLEEP_DELAY);
      if (sleepDelay > 0)
        pQueueError->setSleepDelay(sleepDelay);
      else
        AOS_DEBUGTRACE("Sleep delay for error-executor/sleep-delay is invalid, using default", NULL);

      AAutoPtr<AOSContextQueueThreadPool> pQueuePre(new AOSContextQueue_PreExecutor(
        services, 
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/pre-executor/threads", 16), 
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/pre-executor/queues", 4)
      ), true);
      sleepDelay = services.useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/context-queues/pre-executor/sleep-delay"), DEFAULT_SLEEP_DELAY);
      if (sleepDelay > 0)
        pQueuePre->setSleepDelay(sleepDelay);
      else
        AOS_DEBUGTRACE("Sleep delay for pre-executor/sleep-delay is invalid, using default", NULL);

      AAutoPtr<AOSContextQueueThreadPool> pQueueExecutor(new AOSContextQueue_Executor(
        services, 
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/executor/threads", 64), 
        services.useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/executor/queues", 3)
      ), true);
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

      //a_Release local ownership, context manager now owns the queues and will delete them when done
      pQueuePre.setOwnership(false);
      pQueueIsAvail.setOwnership(false);
      pQueueExecutor.setOwnership(false);
      pQueueError.setOwnership(false);

      //a_Associate queue for the listener
      AOSRequestListener listener(services, AOSContextManager::STATE_PRE_EXECUTE);

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
      pQueuePre->start();
      pQueueIsAvail->start();
      pQueueExecutor->start();
      pQueueError->start();


      //
      //a_Start listener
      //
      listener.startListening();

      //
      //a_Start admin listener
      //
      admin.startAdminListener();

      //
      //a_Server is running, wait for exit request
      //
      DEBUG_MEMORY_SET_START_CHECKPOINT();
      do
      {
        //a_This is the watcher for the main loop, when admin is not running the server has stopped
        AThread::sleep(5000);
      }
      while(!admin.isShutdownRequested() || admin.isRunning());
    }
    catch(AException& ex)
    {
      str.assign("main: ");
      str.append(ex);
      traceMultiline(str, NULL);
      services.useLog().addException(ex);
    }
    catch(std::exception& ex)
    {
      str.assign("main: ");
      str.append(ASWNL(ex.what()));
      traceMultiline(str, NULL);
      services.useLog().addException(ex);
    }
    catch(...)
    {
#ifdef __WINDOWS__
    str.assign(argv[0]);
    str.append(".minidump");
    OFSTRUCT ofile;
    HFILE hFile = ::OpenFile(str.c_str(), &ofile, OF_CREATE|OF_WRITE|OF_SHARE_DENY_WRITE);
    ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), (HANDLE)hFile, MiniDumpNormal, NULL, NULL, NULL);
#endif
      AOS_DEBUGTRACE("main: Unknown exception caught", NULL);
      services.useLog().add(ASWNL("main: Unknown exception caught"), ALog::EXCEPTION);
    }
  }
  catch(AException& ex)
  {
    str.assign("main: ");
    str.append(ex);
    traceMultiline(str, NULL);
  }
  catch(std::exception& ex)
  {
    str.assign("main: ");
    str.append(ASWNL(ex.what()));
    traceMultiline(str, NULL);
  }
  catch(...)
  {
#ifdef __WINDOWS__
    str.assign(argv[0]);
    str.append(".minidump");
    OFSTRUCT ofile;
    HFILE hFile = ::OpenFile(str.c_str(), &ofile, OF_CREATE|OF_WRITE|OF_SHARE_DENY_WRITE);
    ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), (HANDLE)hFile, MiniDumpNormal, NULL, NULL, NULL);
#endif
    AOS_DEBUGTRACE("main: Unknown exception caught", NULL);
  }

  DEBUG_MEMORY_LEAK_ANALYSIS_END();
  
  return 0;
}
