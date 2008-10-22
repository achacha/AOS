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
  ADaemon("AOSWatchDog", "AOSWatchDog", "AObjectServer watchdog service"),
  m_SleepTime(1000),
  m_SleepCycles(10)
{
  memset(&m_ProcessInformation, 0, sizeof(PROCESS_INFORMATION));
  memset(&m_StartupInfo, 0, sizeof(STARTUPINFO)); 

  m_StartupInfo.cb = sizeof(STARTUPINFO);  
  m_StartupInfo.lpTitle = "AObjectServer";  
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
      _stopAObjectServer();
      _startAObjectServer();
    }
    else if (ret == 0)
    {
      //a_Start process
      _startAObjectServer();
    }
    else if (ret < 0)
    {
      thread.setRun(false);
      break;
    }

    //a_Wait a bit
    int i=0;
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
  AFILE_TRACER_DEBUG_SCOPE("_isAObjectServerAlive", (void *)this);
  AHTTPResponseHeader response;
  
  AFile_Socket httpSocket(m_RequestHeader, true);
  try
  {
    AFILE_TRACER_DEBUG_MESSAGE((AString("_isAObjectServerAlive: opening socket: ")+m_RequestHeader.useUrl().getServer()+":"+AString::fromInt(m_RequestHeader.useUrl().getPort())).c_str(), NULL);
    httpSocket.open();
  }
  catch(AException& ex)
  {
    AString str("_isAObjectServerAlive: open socket AException: ");
    ex.emit(str);
    str.append("\r\n");
    m_RequestHeader.emit(str);
    AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);
    return 0;
  }
  catch(...)
  {
    AFILE_TRACER_DEBUG_MESSAGE("_isAObjectServerAlive: Unknown exception trying to connect to the server.\r\n", NULL);
    return 0;
  }

  try
  {
    AFILE_TRACER_DEBUG_MESSAGE("_isAObjectServerAlive: sending HTTP request", NULL);
    m_RequestHeader.toAFile(httpSocket);
    response.fromAFile(httpSocket);
    if (response.getStatusCode() != 200)
    {
      //a_Watched process is dead, restart it
      AFILE_TRACER_DEBUG_MESSAGE("_isAObjectServerAlive: Remote process is in error state (ping response != 200)", NULL);
      return 2;
    }
  }
  catch(AException& ex)
  {
    AString str("_isAObjectServerAlive: HTTP request AException: ");
    ex.emit(str);
    str.append("\r\n");
    m_RequestHeader.emit(str);
    AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);
    return 0;
  }
  catch(...)
  {
    AFILE_TRACER_DEBUG_MESSAGE("_isAObjectServerAlive: Unknown exception sending HTTP request.\r\n", NULL);
    return 0;
  }

  return 1;
}

bool AOSWatchDogDaemon::_init()
{
  //a_Get module filename and add relative location of the INI
  AString thisPath(1536, 256);
  DWORD dwRet = ::GetModuleFileName(NULL, thisPath.startUsingCharPtr(), 1536);
  if (dwRet)
  {
    thisPath.stopUsingCharPtr(dwRet);
  }
  
  m_aosExecutablePath.set(thisPath, false);
  m_aosExecutablePath.useFilename().assign("AObjectServer.exe");
  
  AFilename iniFilename(m_aosExecutablePath);
  iniFilename.useFilename().assign("AOSWatchDog.ini");
  AFILE_TRACER_DEBUG_MESSAGE((AString("Using INI file: ")+iniFilename.toAString()).c_str(), NULL);
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

  //a_Init request header
  AString str;
  if (!m_ini.getValue("config", "aos_server_port", str))
  {
    //a_Config error wait for service stop
    AFILE_TRACER_DEBUG_MESSAGE("callbackMain: AOSWatchDog.ini: missing [config]aos_server_port. Graceful shutdown disabled.", NULL);
    MessageBox(NULL,"callbackMain: AOSWatchDog.ini: missing [config]aos_server_port", "AOSWatchDog:  File Not Found.  Unable to continue, please stop the service.",MB_SERVICE_NOTIFICATION);
    AThread::sleep(5000);

    return false;
  }
  m_RequestHeader.useUrl().parse(ASWNL("http://127.0.0.1/ping"));
  m_RequestHeader.useUrl().setPort(str.toInt());
  m_RequestHeader.setPair(AHTTPHeader::HT_REQ_Host, ASW("127.0.0.1",9));

  //a_Init ADMIN request header
  str.clear();
  if (!m_ini.getValue("config", "aos_admin_port", str))
  {
    //a_Config error wait for service stop
    AFILE_TRACER_DEBUG_MESSAGE("callbackMain: AOSWatchDog.ini: missing [config]aos_admin_port. Waiting for stop.", NULL);
    m_AdminRequestHeader.useUrl().setPort(0);
  }
  else
  {
    m_AdminRequestHeader.useUrl().parse(ASWNL("http://127.0.0.1/admin?command=shutdown"));
    m_AdminRequestHeader.useUrl().setPort(str.toInt());
    m_AdminRequestHeader.setPair(AHTTPHeader::HT_REQ_Host, ASW("127.0.0.1",9));
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

bool AOSWatchDogDaemon::_startAObjectServer()
{
  AFILE_TRACER_DEBUG_SCOPE("_startAObjectServer", (void *)this);

  AString exe, path;
  m_aosExecutablePath.emit(exe);
  m_aosExecutablePath.emitPath(path);
  AFILE_TRACER_DEBUG_MESSAGE((AString("_startAObjectServer:  exe=")+exe).c_str(), NULL);
  AFILE_TRACER_DEBUG_MESSAGE((AString("_startAObjectServer: path=")+path).c_str(), NULL);

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

bool AOSWatchDogDaemon::_stopAObjectServer()
{
  AFILE_TRACER_DEBUG_SCOPE("_stopAObjectServer", (void *)this);
  
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
      AFILE_TRACER_DEBUG_MESSAGE((AString("_stopAObjectServer: opening socket: ")+m_AdminRequestHeader.useUrl().getServer()+":"+AString::fromInt(m_AdminRequestHeader.useUrl().getPort())).c_str(), NULL);
      httpSocket.open();
      m_AdminRequestHeader.toAFile(httpSocket);
      response.fromAFile(httpSocket);
      memset(&m_ProcessInformation, 0, sizeof(PROCESS_INFORMATION));
    }
    catch(AException& ex)
    {
      AString str("_stopAObjectServer: AException: ");
      ex.emit(str);
      AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);
      return _terminateAObjectServer();
    }
    catch(...)
    {
      ARope error("_stopAObjectServer: Unknown exception trying to stop AObjectServer.\r\n");
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
  if (smp_MainServiceThread)
  {
    if (!_stopAObjectServer())
      return 0;
    smp_MainServiceThread->setRun(false);
  }
  else
  {
    AFILE_TRACER_DEBUG_MESSAGE("controlStopPending: Service thread is NULL, doing nothing", NULL);
  }
  return 1;
}
