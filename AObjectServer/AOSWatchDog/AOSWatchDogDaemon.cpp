
#include "AOSWatchDogDaemon.hpp"
#include "AFile_Socket.hpp"
#include "AHTTPRequestHeader.hpp"
#include "AHTTPResponseHeader.hpp"
#include "AINIProfile.hpp"
#include "ASystemException.hpp"
#include "AFileSystem.hpp"

AOSWatchDogDaemon::AOSWatchDogDaemon() :
  ADaemon("AOSWatchDog", "AOS_WatchDog", "AObjectServer watchdog")
{
  memset(&m_ProcessInformation, 0, sizeof(PROCESS_INFORMATION));
  memset(&m_StartupInfo, 0, sizeof(STARTUPINFO)); 

  m_StartupInfo.cb = sizeof(STARTUPINFO);  
  m_StartupInfo.lpTitle = "AObjectServer";  
}

u4 AOSWatchDogDaemon::callbackMain(AThread& thread)
{
  AFILE_TRACER_DEBUG_SCOPE("callbackMain", (void *)&thread);
  AHTTPRequestHeader request;
  AHTTPResponseHeader response;
  const u4 SLEEP_TIME = 1000;
  const u4 SLEEP_TIME_PARTS = 5;
  const u4 TIME_TO_GET_REPLY = 1000;

  //a_Get module filename and add relative location of the INI
  AString thisPath(1536, 256);
  DWORD dwRet = ::GetModuleFileName(NULL, thisPath.startUsingCharPtr(), 1536);
  if (dwRet)
  {
    thisPath.stopUsingCharPtr(dwRet);
  }
  
  m_aosExecutablePath.set(thisPath);
  m_aosExecutablePath.useFilename().assign("AObjectServer.exe");
  
  AFilename iniFilename(m_aosExecutablePath);
  iniFilename.usePathNames().push_back("aos_root");
  iniFilename.useFilename().assign("AOSWatchDog.ini");
  AFILE_TRACER_DEBUG_MESSAGE((AString("Using INI file: ")+iniFilename.toAString()).c_str(), (void *)&thread);
  try
  {
    if (!AFileSystem::exists(iniFilename))
    {
      //a_Config error wait for service stop
      AFILE_TRACER_DEBUG_MESSAGE((AString("File not found: ")+iniFilename.toAString()).c_str(), (void *)&thread);
      MessageBox(NULL,iniFilename.toAString().c_str(),"AOSWatchDog:  File Not Found.  Unable to continue, please stop the service.",MB_SERVICE_NOTIFICATION);
      while (thread.isRun())
        AThread::sleep(SLEEP_TIME);

      thread.setRunning(false);
      return -1;
    }

    AINIProfile ini(iniFilename);
    AString str;
    if (!ini.getValue("config", "aos_server_port", str))
    {
      //a_Config error wait for service stop
      AFILE_TRACER_DEBUG_MESSAGE("AOSWatchDog.ini: missing [config]aos_server_port. Waiting for stop.", (void *)&thread);
      MessageBox(NULL,"AOSWatchDog.ini: missing [config]aos_server_port", "AOSWatchDog:  File Not Found.  Unable to continue, please stop the service.",MB_SERVICE_NOTIFICATION);
      while (thread.isRun())
        AThread::sleep(SLEEP_TIME);

      thread.setRunning(false);
      return -1;
    }

    //a_Build request
    request.useUrl().parse(ASWNL("http://127.0.0.1/ping"));
    request.useUrl().setPort(str.toInt());
    request.setPair(AHTTPHeader::HT_REQ_Host, ASW("localhost",9));
  }
  catch(AException& ex)
  {
    AString error("callbackMain: Exception caught before main loop. Exiting.\r\n");
    ex.emit(error);
    error.append("\r\n", 2);
    request.emit(error);
    AFILE_TRACER_DEBUG_MESSAGE(error.c_str(), (void *)&thread);
    return -1;
  }
  catch(...)
  {
    AString error("callbackMain: Unknown exception caught before main loop. Exiting.\r\n");
    request.emit(error);
    AFILE_TRACER_DEBUG_MESSAGE(error.c_str(), (void *)&thread);
    return -1;
  }

  //a_Start watchdog loop
  thread.setRunning(true);
  while (thread.isRun())
  {
    AFILE_TRACER_DEBUG_MESSAGE("callbackMain: run", (void *)&thread);
    AFile_Socket httpSocket(request.useUrl().getServer(), request.useUrl().getPort(), true);
    try
    {
      AFILE_TRACER_DEBUG_MESSAGE((AString("callbackMain: opening socket: ")+request.useUrl().getServer()+":"+AString::fromInt(request.useUrl().getPort())).c_str(), (void *)&thread);
      httpSocket.open();
    }
    catch(...)
    {
      //a_Watched process is dead, restart it
      ARope error("callbackMain: Remote process is dead (cannot connect)\r\nIf this is during startup, this is normal and watchdog is about to try starting AObjectServer.\r\n");
      request.emit(error);
      AFILE_TRACER_DEBUG_MESSAGE(error.toAString().c_str(), (void *)&thread);
    
      //a_Start process
      _startAObjectServer();

      {
        int i=0;
        while (i++ < SLEEP_TIME_PARTS)
        {
          if (!thread.isRun())
            break;
          AThread::sleep(SLEEP_TIME);
        }
        if (!thread.isRun())
          break;
      }
      continue;
    }

    try
    {
      request.toAFile(httpSocket);
      AThread::sleep(TIME_TO_GET_REPLY);
      response.fromAFile(httpSocket);
      
      if (response.getStatusCode() != 200)
      {
        //a_Watched process is dead, restart it
        AFILE_TRACER_DEBUG_MESSAGE("callbackMain: Remote process is dead (!=200)", (void *)&thread);
      }
      response.clear();

      {
        int i=0;
        while (i++ < SLEEP_TIME_PARTS)
        {
          if (!thread.isRun())
            break;
          AThread::sleep(SLEEP_TIME);
        }
        if (!thread.isRun())
          break;
      }
    }
    catch(AException& ex)
    {
      AString str("callbackMain: AException: ");
      ex.emit(str);
      AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), (void *)&thread);

      {
        int i=0;
        while (i++ < SLEEP_TIME_PARTS)
        {
          if (!thread.isRun())
            break;
          AThread::sleep(SLEEP_TIME);
        }
        if (!thread.isRun())
          break;
      }
    }
    catch(...)
    {
      AFILE_TRACER_DEBUG_MESSAGE("Unknown exception", (void *)&thread);

      {
        int i=0;
        while (i++ < SLEEP_TIME_PARTS)
        {
          if (!thread.isRun())
            break;
          AThread::sleep(SLEEP_TIME);
        }
        if (!thread.isRun())
          break;
      }
    }
  }
  thread.setRunning(false);

  return 1;
}

bool AOSWatchDogDaemon::_startAObjectServer()
{
  AFILE_TRACER_DEBUG_SCOPE("_startAObjectServer", (void *)this);

  AString exe, path;
  m_aosExecutablePath.emit(exe);
  m_aosExecutablePath.emitPath(path);
  AFILE_TRACER_DEBUG_MESSAGE((AString("_startAObjectServer:  exe=")+exe).c_str(), (void *)this);
  AFILE_TRACER_DEBUG_MESSAGE((AString("_startAObjectServer: path=")+path).c_str(), (void *)this);

  BOOL success = ::CreateProcess(
    exe.c_str(),             //LPCTSTR lpApplicationName,
    NULL,                    //LPTSTR lpCommandLine,
    NULL,                    //LPSECURITY_ATTRIBUTES lpProcessAttributes,
    NULL,                    //LPSECURITY_ATTRIBUTES lpThreadAttributes,
    FALSE,                   //BOOL bInheritHandles,
    DETACHED_PROCESS,        //DWORD dwCreationFlags,
    NULL,                    //LPVOID lpEnvironment,
    path.c_str(),            //LPCTSTR lpCurrentDirectory,
    &m_StartupInfo,          //LPSTARTUPINFO lpStartupInfo,
    &m_ProcessInformation    //LPPROCESS_INFORMATION lpProcessInformation
  );

  if (!success)
  {
    //a_Failure
    ASystemException sysEx(::GetLastError());
    AFILE_TRACER_DEBUG_MESSAGE(sysEx.what().toAString().c_str(), NULL);
    return false;
  }

  //a_Success
  AString strDebug("Process started(");
  strDebug.append(m_aosExecutablePath);
  strDebug.append("):PROCINFO:");
  strDebug.append(success ? "TRUE" : "FALSE");
  strDebug.append(": process_id=");
  strDebug.append(AString::fromU4(m_ProcessInformation.dwProcessId));
  strDebug.append("  thread_id=");
  strDebug.append(AString::fromU4(m_ProcessInformation.dwThreadId));
  strDebug.append("  hProcess=");
  strDebug.append(AString::fromPointer((void *)m_ProcessInformation.hProcess));
  strDebug.append("  hThread=");
  strDebug.append(AString::fromPointer((void *)m_ProcessInformation.hThread));
  AFILE_TRACER_DEBUG_MESSAGE(strDebug.c_str(), NULL);
  
  return true;
}
