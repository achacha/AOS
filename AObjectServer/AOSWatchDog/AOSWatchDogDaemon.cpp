/*
Written by Alex Chachanashvili

$Id$
*/
#include "AOSWatchDogDaemon.hpp"
#include "AFile_Socket.hpp"
#include "AHTTPResponseHeader.hpp"
#include "ASystemException.hpp"
#include "AFileSystem.hpp"

AOSWatchDogDaemon::AOSWatchDogDaemon() :
  ADaemon("AOSWatchDog", "AOSWatchDog", "AOS Rhino watchdog service"),
  m_SleepTime(1000),
  m_SleepCycles(10)
{
  memset(&m_ProcessInformation, 0, sizeof(PROCESS_INFORMATION));
  memset(&m_StartupInfo, 0, sizeof(STARTUPINFO)); 

  m_StartupInfo.cb = sizeof(STARTUPINFO);  
  m_StartupInfo.lpTitle = "AOSRhino";  
}

u4 AOSWatchDogDaemon::callbackMain(AThread& thread)
{
  AFILE_TRACER_DEBUG_SCOPE("callbackMain", (void *)&thread);
  AHTTPResponseHeader response;

  //a_Initialize
  if (!_init())
    return -1;

  //a_Start watchdog loop
  thread.setRunning(true);
  while (thread.isRun())
  {
    AFILE_TRACER_DEBUG_MESSAGE("callbackMain: run", (void *)&thread);
    int ret = _isAObjectServerAlive();
    if (ret == 2)
    {
      //a_Process may be up but not responding
      stopServer();
      startServer();
    }
    else if (ret == 0)
    {
      //a_Start process
      startServer();
    }
    else if (ret < 0)
    {
      thread.setRun(false);
      break;
    }

    //a_Wait a bit
    u4 i=0;
    while (i++ < m_SleepCycles)
    {
      if (!thread.isRun())
        break;
      AThread::sleep(m_SleepTime);
    }
    if (!thread.isRun())
      break;
  }
  thread.setRunning(false);

  return 1;
}

int AOSWatchDogDaemon::_isAObjectServerAlive()
{
  AFILE_TRACER_DEBUG_SCOPE("_isAOSAlive", (void *)this);
  AHTTPResponseHeader response;
  
  AFile_Socket httpSocket(m_RequestHeader, true);
  try
  {
    AFILE_TRACER_DEBUG_MESSAGE((AString("_isAOSAlive: opening socket: ")+m_RequestHeader.useUrl().getServer()+":"+AString::fromInt(m_RequestHeader.useUrl().getPort())).c_str(), NULL);
    httpSocket.open();
  }
  catch(AException& ex)
  {
    AString str("_isAOSAlive: open socket AException: ");
    ex.emit(str);
    str.append("\r\n");
    m_RequestHeader.emit(str);
    AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);
    return 0;
  }
  catch(...)
  {
    AFILE_TRACER_DEBUG_MESSAGE("_isAOSAlive: Unknown exception trying to connect to the server.\r\n", NULL);
    return 0;
  }

  try
  {
    AFILE_TRACER_DEBUG_MESSAGE("_isAOSAlive: sending HTTP request", NULL);
    m_RequestHeader.toAFile(httpSocket);
    response.fromAFile(httpSocket);
    if (response.getStatusCode() != 200)
    {
      //a_Watched process is dead, restart it
      AFILE_TRACER_DEBUG_MESSAGE("_isAOSAlive: Remote process is in error state (ping response != 200)", NULL);
      return 2;
    }
  }
  catch(AException& ex)
  {
    AString str("_isAOSAlive: HTTP request AException: ");
    ex.emit(str);
    str.append("\r\n");
    m_RequestHeader.emit(str);
    AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);
    return 0;
  }
  catch(...)
  {
    AFILE_TRACER_DEBUG_MESSAGE("_isAOSAlive: Unknown exception sending HTTP request.\r\n", NULL);
    return 0;
  }

  return 1;
}

bool AOSWatchDogDaemon::_init()
{
  //For debugging use only
  //MessageBox(NULL,"callbackMain: Waiting for debugger to attach", "AOSWatchDog was asked to wait on startup.",MB_SERVICE_NOTIFICATION);

  //a_Get module filename and add relative location of the INI
  AString thisPath(1536, 256);
  DWORD dwRet = ::GetModuleFileName(NULL, thisPath.startUsingCharPtr(), 1536);
  if (dwRet)
  {
    thisPath.stopUsingCharPtr(dwRet);
  }
    
  AFilename iniFilename(thisPath, false);

  AString str;
  iniFilename.useFilename().assign("AOSWatchDog.ini");
  str.assign("Using INI file: ");
  str.append(iniFilename);
  AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);

  try
  {
    m_ini.clear();
    if (!AFileSystem::exists(iniFilename))
    {
      //a_Config error wait for service stop
      AFILE_TRACER_DEBUG_MESSAGE((AString("File not found: ")+iniFilename.toAString()).c_str(), NULL);
      MessageBox(NULL,iniFilename.toAString().c_str(),"AOSWatchDog:  File Not Found.  Unable to continue, please stop the service.",MB_SERVICE_NOTIFICATION);
      AThread::sleep(5000);
      return false;
    }
    m_ini.setFilename(iniFilename);
    m_ini.parse();
  }
  catch(AException& ex)
  {
    AString error("callbackMain: Exception caught in _init: \r\n");
    ex.emit(error);
    AFILE_TRACER_DEBUG_MESSAGE(error.c_str(), NULL);
    return false;
  }
  catch(...)
  {
    AString error("callbackMain: Unknown exception caught in _init.\r\n");
    AFILE_TRACER_DEBUG_MESSAGE(error.c_str(), NULL);
    return false;
  }

  //a_Check if we should wait for debugger to attach
  if (!m_ini.isValue("config", "wait_on_startup", "true"))
    MessageBox(NULL,"callbackMain: Waiting for debugger to attach", "AOSWatchDog was asked to wait on startup.",MB_SERVICE_NOTIFICATION);

  m_aosExecutablePath.set(thisPath, false);
  str.clear();
  if (m_ini.getValue("config", "server_executable", str))
    m_aosExecutablePath.useFilename().assign(str);
  else
    m_aosExecutablePath.useFilename().assign("AObjectServer.exe");

  //
  // Init request header
  //
  str.clear();
  if (!m_ini.getValue("config", "server_isalive_command", str))
  {
    //a_Config error wait for service stop
    AFILE_TRACER_DEBUG_MESSAGE("callbackMain: AOSWatchDog.ini: missing [config]server_isalive_command.  Required for monitoring.", NULL);
    MessageBox(NULL,"callbackMain: AOSWatchDog.ini: missing [config]server_isalive_command", "AOSWatchDog:  Unable to continue.",MB_SERVICE_NOTIFICATION);
    AThread::sleep(5000);

    return false;
  }
  else
  {
    AString hostname;
    m_ini.getValue("config", "server_hostname", hostname);
    AString ip = ASocketLibrary::getIPFromAddress(hostname);
    m_RequestHeader.useUrl().setServer(ip);
    m_RequestHeader.set(AHTTPHeader::HT_REQ_Host, ip);  //HTTP/1.1 reqiured
    m_RequestHeader.useUrl().setProtocol(AUrl::HTTP);

    AString port;
    if (m_ini.getValue("config", "server_port", port))
      m_RequestHeader.useUrl().setPort(port.toInt());
    else
      m_RequestHeader.useUrl().setPort(80);
        
    AUrl overlay(str);
    m_RequestHeader.useUrl().overlay(str);

    str.assign("callbackMain: ISALIVE header configured: ");
    str.append(m_AdminRequestHeader.useUrl());
    str.append(AConstant::ASTRING_CRLF);
    str.append(m_RequestHeader);
    AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);
  }

  //
  // Init ADMIN request header
  //
  str.clear();
  if (!m_ini.getValue("config", "server_shutdown_command", str))
  {
    //a_Config error wait for service stop
    AFILE_TRACER_DEBUG_MESSAGE("callbackMain: AOSWatchDog.ini: missing [config]server_shutdown_url. Server shutdown feature disabled.", NULL);
    m_AdminRequestHeader.useUrl().setPort(0);
  }
  else
  {
    AString hostname;
    m_ini.getValue("config", "server_admin_hostname", hostname);
    AString ip = ASocketLibrary::getIPFromAddress(hostname);
    m_AdminRequestHeader.useUrl().setServer(ip);
    m_AdminRequestHeader.set(AHTTPHeader::HT_REQ_Host, ip);  //HTTP/1.1 reqiured
    m_AdminRequestHeader.useUrl().setProtocol(AUrl::HTTP);

    AString port;
    if (m_ini.getValue("config", "server_admin_port", port))
      m_AdminRequestHeader.useUrl().setPort(port.toInt());
    else
      m_AdminRequestHeader.useUrl().setPort(12345);
        
    AUrl overlay(str);
    m_AdminRequestHeader.useUrl().overlay(overlay);

    str.assign("callbackMain: ADMIN header configured: ");
    str.append(m_AdminRequestHeader.useUrl());
    str.append(AConstant::ASTRING_CRLF);
    str.append(m_AdminRequestHeader);
    AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);
  }

  //a_Timing values
  str.clear();
  if (m_ini.getValue("config", "sleep_cycle_time", str))
    m_SleepTime = str.toU4();

  str.clear();
  if (m_ini.getValue("config", "sleep_cycles", str))
    m_SleepCycles = str.toU4();
  
  return true;
}

bool AOSWatchDogDaemon::startServer()
{
  AFILE_TRACER_DEBUG_SCOPE("startServer", (void *)this);

  AString exe, path;
  m_aosExecutablePath.emit(exe);
  m_aosExecutablePath.emitPath(path);
  AFILE_TRACER_DEBUG_MESSAGE((AString("startServer:  exe=")+exe).c_str(), NULL);
  AFILE_TRACER_DEBUG_MESSAGE((AString("startServer: path=")+path).c_str(), NULL);

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
  AString strDebug("startServer: Process started(");
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

bool AOSWatchDogDaemon::stopServer()
{
  AFILE_TRACER_DEBUG_SCOPE("stopServer", (void *)this);
  
  if (m_AdminRequestHeader.getUrl().getPort() == 0)
  {
    _terminateAObjectServer();
  }
  else
  {
    //a_Signal admin for shutdown
    AHTTPResponseHeader response;
    AFile_Socket httpSocket(m_AdminRequestHeader, true);
    try
    {
      AFILE_TRACER_DEBUG_MESSAGE((AString("stopServer: opening socket: ")+m_AdminRequestHeader.useUrl().getServer()+":"+AString::fromInt(m_AdminRequestHeader.useUrl().getPort())).c_str(), NULL);
      httpSocket.open();
      m_AdminRequestHeader.toAFile(httpSocket);
      response.fromAFile(httpSocket);
      memset(&m_ProcessInformation, 0, sizeof(PROCESS_INFORMATION));
    }
    catch(AException& ex)
    {
      AString str("stopServer: AException: ");
      ex.emit(str);
      AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);
      return _terminateAObjectServer();
    }
    catch(...)
    {
      ARope error("stopServer: Unknown exception trying to stop AObjectServer.\r\n");
      AFILE_TRACER_DEBUG_MESSAGE(error.toAString().c_str(), NULL);
      return false;
    }
  }

  return true;
}

bool AOSWatchDogDaemon::bounceServer()
{
  AFILE_TRACER_DEBUG_SCOPE("bounceServer", (void *)this);
  
  if (m_AdminRequestHeader.getUrl().getPort() == 0)
  {
    _terminateAObjectServer();
  }
  else
  {
    //a_Signal admin for shutdown
    AHTTPResponseHeader response;
    AFile_Socket httpSocket(m_AdminRequestHeader, true);
    try
    {
      AFILE_TRACER_DEBUG_MESSAGE((AString("bounceServer: opening socket: ")+m_AdminRequestHeader.useUrl().getServer()+":"+AString::fromInt(m_AdminRequestHeader.useUrl().getPort())).c_str(), NULL);
      httpSocket.open();
      m_AdminRequestHeader.toAFile(httpSocket);
      response.fromAFile(httpSocket);
      memset(&m_ProcessInformation, 0, sizeof(PROCESS_INFORMATION));
    }
    catch(AException& ex)
    {
      AString str("bounceServer: AException: ");
      ex.emit(str);
      AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);
      return _terminateAObjectServer();
    }
    catch(...)
    {
      ARope error("bounceServer: Unknown exception trying to bounce AObjectServer.\r\n");
      AFILE_TRACER_DEBUG_MESSAGE(error.toAString().c_str(), NULL);
      return false;
    }
  }

  return true;
}

bool AOSWatchDogDaemon::_terminateAObjectServer()
{
  AFILE_TRACER_DEBUG_SCOPE("_terminateAObjectServer", (void *)this);
  if (m_ProcessInformation.hProcess == 0)
  {
    AFILE_TRACER_DEBUG_MESSAGE("_terminateAObjectServer: Unable to terminate process, not creator", (void *)NULL);
    return false;
  }
  else
  {
    if (!::TerminateProcess(m_ProcessInformation.hProcess, -1))
    {
      AFILE_TRACER_DEBUG_MESSAGE("_terminateAObjectServer: Unable to terminate process", (void *)m_ProcessInformation.hProcess);
    }
    else
    {
      AFILE_TRACER_DEBUG_MESSAGE("_terminateAObjectServer: Process terminated", (void *)m_ProcessInformation.hProcess);
      memset(&m_ProcessInformation, 0, sizeof(PROCESS_INFORMATION));
    }
  }
  return true;
}

int AOSWatchDogDaemon::controlStopPending()
{
  AFILE_TRACER_DEBUG_MESSAGE("controlStopPending: Attempting to stop the controlled process", NULL);
  if (g_pMainServiceThread)
  {
    if (!stopServer())
      return 0;
    g_pMainServiceThread->setRun(false);
  }
  else
  {
    AFILE_TRACER_DEBUG_MESSAGE("controlStopPending: Service thread is NULL, doing nothing", NULL);
  }
  return 1;
}
