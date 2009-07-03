/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAObjectServer.hpp"

// This memoy leak detector is pretty fast but only works in Windows 32-bit mode
// Should be enabled to check for leaks
// After using the code, invoke a shutdown from admin console and check debug output window for any leaks
// Ignore the 2 leaks in OpenSSL, they are process lifetime leaks and I have not found an API call to clean them up yet
//#define __ENABLE_MEMLEAK_DETECT__
#if !defined(NDEBUG) && defined(WIN32) && !defined(WIN64) && defined(__ENABLE_MEMLEAK_DETECT__)
#include "MemLeakDetect.h"
CMemLeakDetect memLeakDetect;
#else
#if defined(__WINDOWS__)
#include "dbghelp.h"
#include "dbgeng.h"
#pragma comment(lib, "dbghelp")
#pragma comment(lib, "dbgeng")
#endif
#endif

#include "AOS.hpp"
#include "AOSAdmin.hpp"
#include "AOSServices.hpp"
#include "ASocketLibrary_SSL.hpp"
#include "AGdLibrary.hpp"

#include "AOSRequestListener.hpp"
#include "AOSContextQueue_IsAvailable.hpp"
#include "AOSContextQueue_PreExecutor.hpp"
#include "AOSContextQueue_Executor.hpp"
#include "AOSContextQueue_ErrorExecutor.hpp"

ASocketLibrary_SSL g_SecureSocketLibrary;    // Global init for SSL and socket library
AGdLibrary g_GdLibrary;                      // Global init for gd library

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
  // AString available for errors so that allocation is not done during exception (in case we ran out of memory, etc)
  AString str(2048, 512);
#if defined(__WINDOWS__) && defined(WINDOWS_CRTDBG_ENABLED)
  DEBUG_MEMORY_LEAK_ANALYSIS_BEGIN(false);

  LPAPI_VERSION lpVersion = ::ImagehlpApiVersion();
  if (lpVersion->MajorVersion < 4)
  {
    AOS_DEBUGTRACE("dgbhelp.dll major version < 4 (check your path to make sure the latest dbghelp.dll is picked up if removed from current directory)", NULL);
    return -1;
  }
#endif

  AOS_DEBUGTRACE(AString(AOS_Base_INFO).c_str(), NULL);
  
  {
    AFilename f;
    AFileSystem::getCWD(f);
    AOS_DEBUGTRACE((AString("Current working directory: ")+f).c_str(), NULL);
  }
  
  try
  {
    // Check if config base path was provided
    AFilename basePath(ASW("../aos_root/",11), true);
    if (argc > 1)
    {
      // Check existance of config base path
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
      AString str("Base 'aos_root' directory: ");
      absPath.emit(str);
      AOS_DEBUGTRACE(str.c_str(), NULL);
    }

    //
    // Initialize services
    //
    AAutoPtr<AOSServices> pservices(new AOSServices(basePath), true);
        
    try
    {
      //
      // Initialize services
      //
      pservices->init();
      
      //
      // Initialize admin objects
      //
      AOSAdmin admin(*pservices);
      pservices->adminRegisterObject(pservices->useAdminRegistry());

      //
      // Initialize the global objects
      //
      AString strError;
      bool boolDatabaseInitialized = false;
      int retriesLeft = 11;
      const int DBCONNECT_TIMEOUT = 1000;  // milliseconds per retry
      while (!boolDatabaseInitialized)
      {
        if (!--retriesLeft)
        {
          //a_Failed to connect to the database
          ATHROW_EX(NULL, AException::OperationFailed, ASWNL("Failed to initialized the database after 10 retries, check database nd connection URL"));
        }

        try
        {
          boolDatabaseInitialized = pservices->initDatabasePool();
          if (boolDatabaseInitialized)
          {
            size_t rows = pservices->loadGlobalObjects(strError);
            if (AConstant::npos == rows)
            {
              AOS_DEBUGTRACE((AString("DBERROR: Global init: ")+strError).c_str(), NULL);
              return -1;
            }
            else
            {
              str.clear();
              str.assign("Database connection pool connected ");
              pservices->useConfiguration().useConfigRoot().emitString(AOSConfiguration::DATABASE_CONNECTIONS, str);
              str.append("x to ");
              pservices->useConfiguration().useConfigRoot().emitString(AOSConfiguration::DATABASE_URL, str);
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
        }
        catch(AException& ex)
        {
          str.assign("main.dbinit: ");
          str.append(ex);
          traceMultiline(str, NULL);
          pservices->useLog().addException(ex);
          AThread::sleep(DBCONNECT_TIMEOUT);
        }
      }

      //
      // Load the processors, modules, generators dynamically from DLLs
      //
      if (!pservices->loadModules())
      {
        AOS_DEBUGTRACE("ERROR: Failed to load modules, check the log for details.", NULL);
        return -1;
      }

      //
      // Create and configure the queues
      //
      AAutoPtr<AOSContextQueueInterface> pQueueIsAvail(new AOSContextQueue_IsAvailable(
        *pservices,
        pservices->useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/is-available/queues", 2)
      ), true);
      
      AAutoPtr<AOSContextQueueThreadPool> pQueueError(new AOSContextQueue_ErrorExecutor(
        *pservices,
        pservices->useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/error-executor/threads", 16), 
        pservices->useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/error-executor/queues", 4)
      ), true);
      int sleepDelay = pservices->useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/context-queues/error-executor/sleep-delay"), DEFAULT_SLEEP_DELAY);
      if (sleepDelay > 0)
        pQueueError->setSleepDelay(sleepDelay);
      else
        AOS_DEBUGTRACE("Sleep delay for error-executor/sleep-delay is invalid, using default", NULL);

      AAutoPtr<AOSContextQueueThreadPool> pQueuePre(new AOSContextQueue_PreExecutor(
        *pservices, 
        pservices->useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/pre-executor/threads", 16), 
        pservices->useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/pre-executor/queues", 4)
      ), true);
      sleepDelay = pservices->useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/context-queues/pre-executor/sleep-delay"), DEFAULT_SLEEP_DELAY);
      if (sleepDelay > 0)
        pQueuePre->setSleepDelay(sleepDelay);
      else
        AOS_DEBUGTRACE("Sleep delay for pre-executor/sleep-delay is invalid, using default", NULL);

      AAutoPtr<AOSContextQueueThreadPool> pQueueExecutor(new AOSContextQueue_Executor(
        *pservices, 
        pservices->useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/executor/threads", 64), 
        pservices->useConfiguration().useConfigRoot().getSize_t("/config/server/context-queues/executor/queues", 3)
      ), true);
      sleepDelay = pservices->useConfiguration().useConfigRoot().getInt(ASWNL("/config/server/context-queues/executor/sleep-delay"), DEFAULT_SLEEP_DELAY);
      if (sleepDelay > 0)
        pQueueExecutor->setSleepDelay(sleepDelay);
      else
        AOS_DEBUGTRACE("Sleep delay for executor/sleep-delay is invalid, using default", NULL);

      // Configure queue state machine
      pservices->useContextManager().setQueueForState(AOSContextManager::STATE_PRE_EXECUTE, pQueuePre);
      pservices->useContextManager().setQueueForState(AOSContextManager::STATE_EXECUTE, pQueueExecutor);
      pservices->useContextManager().setQueueForState(AOSContextManager::STATE_IS_AVAILABLE, pQueueIsAvail);
      pservices->useContextManager().setQueueForState(AOSContextManager::STATE_ERROR, pQueueError);

      // Release local ownership, context manager now owns the queues and will delete them when done
      pQueuePre.setOwnership(false);
      pQueueIsAvail.setOwnership(false);
      pQueueExecutor.setOwnership(false);
      pQueueError.setOwnership(false);

      // Associate queue for the listener
      AOSRequestListener listener(*pservices, AOSContextManager::STATE_PRE_EXECUTE);

      //
      // Start all the queues (listener is the last thing to start, see below)
      //
      pQueuePre->start();
      pQueueIsAvail->start();
      pQueueExecutor->start();
      pQueueError->start();


      //
      // Start listener
      //
      listener.startListening();

      //
      // Start admin listener
      //
      admin.startAdminListener();

      //
      // Server is running, wait for exit request
      //
#if defined(__WINDOWS__) && defined(WINDOWS_CRTDBG_ENABLED)
      DEBUG_MEMORY_SET_START_CHECKPOINT();
#endif
      
      str.clear(true);
      
      u4 sleepTime = pservices->useConfiguration().useConfigRoot().getU4("/server/main/sleep", 3000);
      AASSERT(NULL, sleepTime > 100);  // Should never bee lower than 100 ms, waste of cycles
      do
      {
        // This is the watcher for the main loop, when admin is not running the server has stopped
        AThread::sleep(sleepTime);
      }
      while(!admin.isShutdownRequested() || admin.isRunning());
    }
    catch(AException& ex)
    {
      str.assign("main: ");
      str.append(ex);
      traceMultiline(str, NULL);
      pservices->useLog().addException(ex);
      return -1;
    }
    catch(std::exception& ex)
    {
      str.assign("main: ");
      str.append(ASWNL(ex.what()));
      traceMultiline(str, NULL);
      pservices->useLog().addException(ex);
      return -1;
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
      pservices->useLog().add(ASWNL("main: Unknown exception caught"), ALog::EVENT_EXCEPTION);
      return -1;
    }
  }
  catch(AException& ex)
  {
    str.assign("main caught AException: ");
    str.append(ex);
    str.append("\r\nWaiting on all threads to exit, may take a few seconds.\r\n");
    traceMultiline(str, NULL);
    return -1;
  }
  catch(std::exception& ex)
  {
    str.assign("main: ");
    str.append(ASWNL(ex.what()));
    str.append("\r\nWaiting on all threads to exit, may take a few seconds.\r\n");
    traceMultiline(str, NULL);
    return -1;
  }
  catch(...)
  {
#ifdef __WINDOWS__
    str.assign("main: Unknown exception caught. Minidump: ");
    str.append(argv[0]);
    str.append(".minidump");
    OFSTRUCT ofile;
    HFILE hFile = ::OpenFile(str.c_str(), &ofile, OF_CREATE|OF_WRITE|OF_SHARE_DENY_WRITE);
    ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), (HANDLE)hFile, MiniDumpNormal, NULL, NULL, NULL);
#else
    str.assign("main: Unknown exception caught.");
#endif
    str.append("\r\nWaiting on all threads to exit, may take a few seconds.\r\n");
    AOS_DEBUGTRACE("main: Unknown exception caught", NULL);
    return -1;
  }

#if defined(__WINDOWS__) && defined(WINDOWS_CRTDBG_ENABLED)
  DEBUG_MEMORY_LEAK_ANALYSIS_END();
#endif

  return 0;
}
