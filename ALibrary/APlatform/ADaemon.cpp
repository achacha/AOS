/*
Written by Alex Chachanashvili

$Id: ADaemon.cpp 303 2009-09-16 16:05:07Z achacha $
*/
#include "pchAPlatform.hpp"
#include "ADaemon.hpp"
#include "ASystemException.hpp"

#ifdef __LINUX__
#include <sys/stat.h>
#endif

ADaemon *thisADaemon = NULL;

#ifdef __WINDOWS__
AThread *ADaemon::mp_MainServiceThread = NULL;
//a_Warnings due to conditional compile with unix code
#pragma warning (disable:4100)  // unreferenced formal parameter
#pragma warning (disable:4101)  // unreferenced local variable

#pragma message("Compiling Windows service code.")

#define STOP_TIMEOUT 10        // 10 seconds timeout to stop service on removal

DWORD WINAPI fcbServiceHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
  AFILE_TRACER_DEBUG_SCOPE("fcbServiceHandler", NULL);

  thisADaemon->init();
  SERVICE_STATUS ssNow;
  if (!thisADaemon->getNTServiceStatus(ssNow))
    return NO_ERROR;     //a_Problem getting status

  try
  {
    switch(dwControl)
    {
      case SERVICE_CONTROL_INTERROGATE:
      {
        AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:INTERROGATE", NULL);
        if (thisADaemon->controlInterrogate())
         thisADaemon->notifyServiceControlManager(ssNow.dwCurrentState, 0);
      }
      break;

      case SERVICE_CONTROL_STOP:
        AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:STOP: Flagging thread to stop", NULL);
        AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler: flagging MainServiceThread to stop running", NULL);
        thisADaemon->mp_MainServiceThread->setRun(false);
        if (thisADaemon->controlStopPending() == 0)
        {
          //a_Abort requested
          AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler: controlStopPending request abort", NULL);
          thisADaemon->notifyServiceControlManager(ssNow.dwCurrentState, 1);
          return NO_ERROR;
        }
        else
        {
          const int iiTimeout = 20;
          int i=0;
          while (thisADaemon->mp_MainServiceThread->isRunning() && ++i < iiTimeout)
          {
            AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler: Waiting for service thread to stop", NULL);
            AThread::sleep(1000);
          }

          if (i >= iiTimeout)
          {
            AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:STOP: Sledgehammer to the thread, timeout detected", NULL);
            thisADaemon->mp_MainServiceThread->terminate();       //a_Sledgehammer after timeout
          }
        }

        if (thisADaemon->controlStopped())
          thisADaemon->notifyServiceControlManager(SERVICE_STOPPED, 0);
        AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:STOP: Stopped", NULL);
     break;

      case SERVICE_CONTROL_SHUTDOWN:
        AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:SHUTDOWN", NULL);
        thisADaemon->controlShutdown();
      break;

      case SERVICE_CONTROL_PAUSE:
        AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:PAUSE", NULL);
        thisADaemon->notifyServiceControlManager(SERVICE_PAUSE_PENDING, 1);
        if (thisADaemon->controlPausePending() == 0)
        {
          //a_Abort requested
          thisADaemon->notifyServiceControlManager(ssNow.dwCurrentState, 1);
          return NO_ERROR;
        }

        if (thisADaemon->mp_MainServiceThread)
          thisADaemon->mp_MainServiceThread->suspend();
        thisADaemon->notifyServiceControlManager(SERVICE_PAUSED, 0);
        thisADaemon->controlPaused();
      break;

      case SERVICE_CONTROL_CONTINUE:
        AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:CONTINUE", NULL);
        if (thisADaemon->mp_MainServiceThread)
          thisADaemon->mp_MainServiceThread->resume();
        thisADaemon->notifyServiceControlManager(SERVICE_CONTINUE_PENDING, 1);
        if (thisADaemon->controlContinuePending() == 0)
        {
          //a_Abort requested
          thisADaemon->notifyServiceControlManager(ssNow.dwCurrentState, 1);
          return NO_ERROR;
        }

        thisADaemon->notifyServiceControlManager(SERVICE_RUNNING, 0);
        thisADaemon->controlRunning();
      break;

      default:
        AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:UNHANDLED", NULL);
    }
  }
  catch(AException &eX)
  {
    MessageBox(NULL,eX.what().toAString().c_str(),"fcbServiceHandler",MB_SERVICE_NOTIFICATION);
    AFILE_TRACER_DEBUG_MESSAGE(eX.what().toAString().c_str(), NULL);
  }

  return NO_ERROR;
}
#else
void fcbServiceHandler(u4 dwControl, u4 dwEventType, void *lpEventData, void *lpContext)
{
  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler called.", NULL);

  return;
}
#endif

//a_Main function called that will try to start a service
#if defined(__WINDOWS__)
#ifdef UNICODE
VOID WINAPI fcbServiceMain(DWORD argc, LPTSTR *argv)
#else
VOID WINAPI fcbServiceMain(DWORD argc, LPSTR *argv)
#endif
{
  AFILE_TRACER_DEBUG_SCOPE("fcbServiceMain", NULL);
  if (!thisADaemon)
  {
    MessageBox(NULL,"thisADaemon is NULL, this means service class was never created in this instance or went out of scope, should be static or global","ADaemon::fcbServiceMain",MB_SERVICE_NOTIFICATION);
    return;
  }

  if(!(thisADaemon->sm_sshService = ::RegisterServiceCtrlHandlerEx(thisADaemon->m_serviceName.c_str(), (LPHANDLER_FUNCTION_EX)fcbServiceHandler, NULL)))
  {
    AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain failed to register service handler.", NULL);
    return;
  }
  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Handler registered", NULL);

  //a_This will make the litle clock go by in the dialog
  thisADaemon->notifyServiceControlManager(SERVICE_START_PENDING, 1);

  //a_Start pending, start-up code should go here
  if (thisADaemon->controlStartPending() == 0)
    return;                           //a_Abort requested

  //a_Update the clock display if needed
  thisADaemon->notifyServiceControlManager(SERVICE_START_PENDING, 2);

  //a_Create a new thread for the service
  try
  {
    delete thisADaemon->mp_MainServiceThread;      //a_This should always just return, as it should point to NULL
    thisADaemon->mp_MainServiceThread = new AThread((AThread::ATHREAD_PROC *)ADaemon::MainServiceThreadProc, false);
    AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Service thread created", NULL);
  }
  catch(AException &eX)
  {
    AFILE_TRACER_DEBUG_MESSAGE(eX.what().toAString().c_str(), NULL);
    MessageBox(NULL,eX.what().toAString().c_str(),"fcbServiceMain",MB_SERVICE_NOTIFICATION);
    return;
  }
  catch(...)
  {
    MessageBox(NULL,"Unknown Exception Caught","fcbServiceMain",MB_SERVICE_NOTIFICATION);
    return;
  }

  thisADaemon->notifyServiceControlManager(SERVICE_START_PENDING, 3);
  thisADaemon->mp_MainServiceThread->setRun(true);  //a_Set thread to Run, the thread will in turn set Running flag to true when it is actually running
  thisADaemon->mp_MainServiceThread->start();

  u4 progress = 5;
  while (thisADaemon->mp_MainServiceThread->isRun() && thisADaemon->mp_MainServiceThread->isRunning())
  {
    AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Service waiting for thread to start", NULL);
    AThread::sleep(500);
    thisADaemon->notifyServiceControlManager(SERVICE_START_PENDING, progress++);
  }

  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Service thread started", NULL);
  thisADaemon->notifyServiceControlManager(SERVICE_RUNNING, 0);
  thisADaemon->controlRunning();

  //a_Wait until done
  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Starting to wait for signal of termination.", NULL);

  //a_Wait until main thread finishes
  do
  {
    AThread::sleep(1000);
  }
  while(thisADaemon->mp_MainServiceThread->isRunning());


  //a_Finish the thread and delete
  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Thread stop signaled", NULL);

  if (thisADaemon->mp_MainServiceThread->isRunning())
  {
    AFILE_TRACER_DEBUG_MESSAGE((AString("fcbServiceMain: Terminating thread: ") + AString::fromPointer(thisADaemon->mp_MainServiceThread)).c_str(), NULL);
    thisADaemon->mp_MainServiceThread->terminate();
  }

  delete thisADaemon->mp_MainServiceThread;
  thisADaemon->mp_MainServiceThread = NULL;

  return;
}
#else

void fcbServiceMain(u4 dwArgc, char **lpszArgv)
{
  //a_UNIX (sun) code for the service
  //a_Start pending, start-up code should go here
  AFILE_TRACER_DEBUG_SCOPE("fcbServiceMain", NULL);

  //a_Create a new thread for the service
  try
  {
    delete thisADaemon->mp_MainServiceThread;      //a_This should always just return, as it should point to NULL
    thisADaemon->mp_MainServiceThread = new AThread((AThread::ATHREAD_PROC *)ADaemon::MainServiceThreadProc, true);
  }
  catch(AException &eX)
  {
    AFILE_TRACER_DEBUG_MESSAGE(eX.what(), NULL);
    return;
  }

  thisADaemon->mp_MainServiceThread->setRun();

  //a_Wait until done
  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain::Starting to wait for signal of termination.", NULL);
  while(thisADaemon->mp_MainServiceThread->isRunning())
    AThread::sleep(2000);

  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain::Exiting!", NULL);

  if (thisADaemon->mp_MainServiceThread->isRunning())
  {
  //a_Finish the thread and delete
    thisADaemon->mp_MainServiceThread->terminate();
    AFILE_TRACER_DEBUG_MESSAGE((AString("fcbServiceMain::Terminating thread: ") + AString::fromPointer(thisADaemon->mp_MainServiceThread)).c_str(), NULL);
  }
  delete thisADaemon->mp_MainServiceThread;
  thisADaemon->mp_MainServiceThread = NULL;

  return;
}

#endif

u4 ADaemon::MainServiceThreadProc(AThread& thread)
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::MainServiceThreadProc", NULL);
  u4 ret = 0;
  if (!thisADaemon)
  {
#ifdef __WINDOWS__
    MessageBox(NULL,"ADaemon global object does not exist, callbackMain not being called.","ADaemon::MainServiceThreadProc",MB_SERVICE_NOTIFICATION);
#else
    std::cerr << "ADaemon global object does not exist, callbackMain not being called." << std::endl;
#endif
    return (u4)-1;
  }
  else
  {
    thread.setRunning(true);
    AFILE_TRACER_DEBUG_SCOPE("MainServiceThreadProc: ADaemon::callbackMain", NULL);
    ret = thisADaemon->callbackMain(thread);
  }
  AFILE_TRACER_DEBUG_MESSAGE("MainServiceThreadProc: callbackMain flagged as not running", NULL);
  thread.setRunning(false);
  return ret;
}

/////////////////////////////////////////////////////////////////////////////////////
ADaemon::ADaemon(const AString& serviceName, const AString& displayName, const AString& description) :
  m_serviceName(serviceName),
  m_displayName(displayName),
  m_description(description)
{
#if defined(__WINDOWS__)
  m_steServiceTable[0].lpServiceName = m_serviceName.startUsingCharPtr();
  m_steServiceTable[0].lpServiceProc = fcbServiceMain; //-V525
  m_steServiceTable[1].lpServiceName = NULL;         //a_Must be NULL
  m_steServiceTable[1].lpServiceProc = NULL;         //a_Must be NULL

  m_schService = NULL;                               //a_ServiceControl service
  m_schManager = NULL;                               //a_ServiceControl manager
  sm_sshService = NULL;

  //a_Connects this service to a service dispatcher if it exists
  m_iServiceMode = 0;

  AASSERT(NULL, !thisADaemon);      //a_Only one instance per process
  thisADaemon = this;

#else
  //a_UNIX only
  m_pidParent = 0;                                     //a_No PID yet
#endif
}

ADaemon::~ADaemon()
{
  try
  {
#if defined(__WINDOWS__)
  _closeService();                                    //a_If it needs to, it will close itself
#endif
  }
  catch(...) {}

  thisADaemon = this;
}

//a_This function will throw an AException if something fails
//a_Generally a wrapper that will start a daemon (in NT it will install it if missing)
//a_Use NT specific methods for more control over this process
//a_Return values as follow:
//a_ -1 error occured (UNIX and NT)
//a_  0 parent returning, child forked (UNIX) or service started (NT)
//a_  >0 child returning its PID (UNIX only)
//a_dwParam is used by NT as a parameter for the thread
//a_dwParam and ppcParam and used by UNIX as argc and argv that main() usually gets
int ADaemon::invoke(u4 dwParam, char **ppcParam)
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::invoke", this);
#if defined(__WINDOWS__)
//a_NT Service version (does not provide exception re-throw for simplicity)
  if (existNTService())
  {
    //a_Service exists, try starting it
    try
    {
      if (!startNTService())
        return -1;
    }
    catch(AException &)
    {
      //a_Unable to start
      return -1;
    }
    return 0;     //a_Started
  }
  else
  {
    try
    {
      installNTService();
      startNTService();
    }
    catch(AException &)
    {
      return -1;
    }
  }
  return 0;
#else
//a_UNIX version
  //a_Attempt to fork this process, if it fails throw as exception
  if ((m_pidParent = fork()) < 0)
    ATHROW_EX(NULL, AException::OperationFailed, ASWNL("ADaemon::invokeUnable to fork process."));
  else
    if (m_pidParent != 0)
      return 0;   //a_Process forked.  Current parent process terminating.

  //a_We are here thanx to the child, the parent was terminated with the return above
  AFILE_TRACER_DEBUG_MESSAGE("ADaemon::invoke: setting up child", NULL);

  setsid();     //a_We are now the session leader
  chdir("/");   //a_Release control of any subdirectories
  umask(0);    //a_Clear the file mode creation flags
  m_pidChild = getpid();     //a_Get the child's PID

  //a_Execute the main function for the fork
  AFILE_TRACER_DEBUG_MESSAGE("ADaemon::invoke: calling m_pdcbMain", NULL);

  fcbServiceMain(dwParam, ppcParam);   //a_Call the main process

  AFILE_TRACER_DEBUG_MESSAGE("ADaemon::invoke: returning", NULL);

  return m_pidChild;                    //a_The child finished
#endif

}

void ADaemon::init()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::init", this);
#if defined(__WINDOWS__)
  _connectNTServiceToDispatcher();
#endif
}

void ADaemon::wait()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::wait", this);
#if defined(__WINDOWS__)
  if (thisADaemon->mp_MainServiceThread)
    thisADaemon->mp_MainServiceThread->waitForThreadToExit();
#endif
}

#if defined(__WINDOWS__)
int ADaemon::getNTServiceStatus(SERVICE_STATUS &ssNow)
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::getNTServiceStatus", this);
  try
  {
    _openService();
    _getNTServiceStatus(ssNow);
  }
  catch(AException &)
  {
    u4 lastOSError = ::GetLastError();
    _closeService();
    ATHROW_LAST_OS_ERROR_KNOWN(NULL, lastOSError);
  }

  _closeService();
  return 1;
}

int ADaemon::_getNTServiceStatus(SERVICE_STATUS &ssService)
{
  if (!::QueryServiceStatus(m_schService, &ssService))
  {
    _closeService();
    ATHROW_LAST_OS_ERROR(NULL);
  }

  //a_At this point we have the status structure filled
  return 1;
  //a_NOTE: Service will not be closed here unless something went wrong
}

int ADaemon::_openService()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::_openService", this);

  if (m_schManager && m_schService)
  {
    AFILE_TRACER_DEBUG_MESSAGE("ADaemon::_openService: Service already opened", this);
    return 1;
  }

  m_schManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (NULL == m_schManager)
  {
    AFILE_TRACER_DEBUG_MESSAGE("ADaemon::_openService: Unable to open service control manager", this);
    ATHROW_LAST_OS_ERROR(NULL);
  }

  AFILE_TRACER_DEBUG_MESSAGE((ASWNL("ADaemon::_openService: Trying to open service: ")+m_serviceName).c_str(), this);
  m_schService = ::OpenService(m_schManager, m_serviceName.c_str(), SERVICE_ALL_ACCESS);
  if (NULL == m_schService)
  {
    //a_Open service failed
    AFILE_TRACER_DEBUG_MESSAGE("ADaemon::_openService: Unable to open service", this);
    u4 lastOSError = ::GetLastError();
    _closeService();
    ATHROW_LAST_OS_ERROR_KNOWN_EX(NULL, lastOSError, AString("Unable to open service: ")+m_serviceName);
  }
  AFILE_TRACER_DEBUG_MESSAGE((ASWNL("ADaemon::_openService: Service opened: ")+m_serviceName).c_str(), this);

  return 1;
}

int ADaemon::_closeService()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::_closeService", this);

  //a_All done, clean up
  if (m_schService)
  {
    ::CloseServiceHandle(m_schService);     //a_Close the service
    m_schService = NULL;
  }
  if (m_schManager)
  {
    ::CloseServiceHandle(m_schManager);     //a_Close the manager
    m_schManager = NULL;
  }

  return 1;
}

int ADaemon::_controlService(u4 dwParam)
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::_controlService", this);

  //a_Open the service if needed
  SERVICE_STATUS ssMe;
  memset(&ssMe, 0, sizeof(SERVICE_STATUS));
  if (_getNTServiceStatus(ssMe) > 0)   //a_This will throw an exception to be caugth by caller of this function
  {
    //a_Got the structure filled
    if (0 == ControlService(m_schService, dwParam, &ssMe))
    {
      AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::_controlService: Unable to control service: GetLastError=")+AString::fromU4(::GetLastError())).c_str(), this);
      return 0;
    }
  }

  return 1;
  //a_NOTE: Service is not closed here unless something went wrong
}

//---------------------------------------------------------------------------------

int ADaemon::existNTService()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::existNTService", this);
  try
  {
    _openService();
  }
  catch(AException &eX)
  {
    AString str;
    eX.getDescription(str);
    if (AConstant::npos != str.find("exist"))
      return 0;     //a_Does not exist

    throw;          //a_A different error, re-throw exception
  }

  _closeService();
  return 1;
}

AString ADaemon::getStatus()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::getStatus", this);
  try
  {
    _openService();
  }
  catch(AException &eX)
  {
    AString str;
    eX.getDescription(str);
    if (AConstant::npos != str.find("exist"))
      return AString("STATUS: Service does not exist.");     //a_Does not exist

    throw;          //a_A different error, re-throw exception
  }

  //a_Now get the state
  SERVICE_STATUS ssX;
  memset(&ssX, 0, sizeof(SERVICE_STATUS));
  try
  {
    _getNTServiceStatus(ssX);
  }
  catch(AException &)
  {
    return AString("STATUS: Unable to get ServiceStatus.");
  }

  //a_Figure out what the state is
  AString ret;
  switch(ssX.dwCurrentState)
  {
    case SERVICE_STOPPED : ret = "STATUS: The service is not running."; break;
    case SERVICE_START_PENDING : ret = "STATUS: The service is starting."; break;
    case SERVICE_STOP_PENDING : ret = "STATUS: The service is stopping."; break;
    case SERVICE_RUNNING : ret = "STATUS: The service is running."; break;
    case SERVICE_CONTINUE_PENDING : ret = "STATUS: The service continue is pending."; break;
    case SERVICE_PAUSE_PENDING : ret = "STATUS: The service pause is pending."; break;
    case SERVICE_PAUSED : ret = "STATUS: The service is paused."; break;
    default : ret = "STATUS: Unknown.";
  }

  _closeService();
  return ret;
}

int ADaemon::notifyServiceControlManager(
  u4 dwState,
  u4 dwProgress,
  u4 dwWaitHint   // = 1000
)
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::notifyServiceControlManager", this);

  SERVICE_STATUS ssX;

  ssX.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  ssX.dwControlsAccepted = SERVICE_CONTROL_INTERROGATE;
  ssX.dwCheckPoint = dwProgress;
  ssX.dwCurrentState = dwState;
  ssX.dwWin32ExitCode = NO_ERROR;
  ssX.dwServiceSpecificExitCode = 0;
  ssX.dwWaitHint = dwWaitHint;

  switch(dwState)
  {
    case SERVICE_START_PENDING :
    {
      ssX.dwControlsAccepted = 0;
      AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::notifyServiceControlManager: SERVICE_START_PENDING: ")+AString::fromU4(dwProgress)).c_str(), this);
    }
    break;

    case SERVICE_STOP_PENDING :
    {
      ssX.dwControlsAccepted = 0;
      AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::notifyServiceControlManager: SERVICE_STOP_PENDING: ")+AString::fromU4(dwProgress)).c_str(), this);
    }
    break;

    case SERVICE_CONTINUE_PENDING :
    {
      AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::notifyServiceControlManager: SERVICE_CONTINUE_PENDING: ")+AString::fromU4(dwProgress)).c_str(), this);
    }
    break;

    case SERVICE_PAUSE_PENDING :
    {
      AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::notifyServiceControlManager: SERVICE_PAUSE_PENDING: ")+AString::fromU4(dwProgress)).c_str(), this);
    }
    break;

    case SERVICE_STOPPED :
    {
      ssX.dwWaitHint = 0;
      ssX.dwCheckPoint = 0;
      AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::notifyServiceControlManager: SERVICE_STOPPED: ")+AString::fromU4(dwProgress)).c_str(), this);
    }
    break;

    case SERVICE_RUNNING :
    {
      ssX.dwControlsAccepted |= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
      AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::notifyServiceControlManager: SERVICE_RUNNING: ")+AString::fromU4(dwProgress)).c_str(), this);
    }
    break;

    case SERVICE_PAUSED :
    {
      ssX.dwControlsAccepted |= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
      AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::notifyServiceControlManager: SERVICE_PAUSED: ")+AString::fromU4(dwProgress)).c_str(), this);
    }
    break;

    default :
    {
      AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::notifyServiceControlManager: DEFAULT: ")+AString::fromU4(dwState)).c_str(), this);
    }
  }

  int ret = ::SetServiceStatus(sm_sshService, &ssX);
  if (0 == ret)
  {
    int lastError = ::GetLastError();
    AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::notifyServiceControlManager: SetServiceStatus=")+AString::fromInt(ret)+"  GetLastError="+AString::fromInt(lastError)).c_str(), this);
//    ATHROW_LAST_OS_ERROR_KNOWN(NULL, lastError);
  }
  return ret;
}

int ADaemon::_connectNTServiceToDispatcher()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::_connectNTServiceToDispatcher", this);

  //a_Connect to service controller using the current service table
  AFILE_TRACER_DEBUG_MESSAGE((ASWNL("ADaemon::_connectNTServiceToDispatcher: Calling StartServiceCtrlDispatcher: ")+m_steServiceTable[0].lpServiceName).c_str(), this);
  if(!::StartServiceCtrlDispatcher(m_steServiceTable))
  {
    int lastError = ::GetLastError();
    switch(lastError)
    {
      case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT :
        AFILE_TRACER_DEBUG_MESSAGE("ADaemon::_connectNTServiceToDispatcher: console mode", this);
        m_iServiceMode = 0;
      return 0;                   //a_Could not connect with controller, console mode

      case ERROR_INVALID_DATA :
        AFILE_TRACER_DEBUG_MESSAGE("ADaemon::_connectNTServiceToDispatcher: Invalid data", this);
        ATHROW(NULL, AException::InvalidData);

      case ERROR_SERVICE_ALREADY_RUNNING:
        AFILE_TRACER_DEBUG_MESSAGE("ADaemon::_connectNTServiceToDispatcher: Service already running", this);
        break;

      default :
        AFILE_TRACER_DEBUG_MESSAGE((AString("ADaemon::_connectNTServiceToDispatcher: Unknown error: LastError=")+AString::fromInt(lastError)).c_str(), this);
        return -1;    //a_Some other unknown error?!?
    }
  }

  m_iServiceMode = 1;           //a_Service mode

  AFILE_TRACER_DEBUG_MESSAGE("ADaemon::_connectNTServiceToDispatcher: Service connected to controller.", this);

  return 1;
}

int ADaemon::installNTService(const AString &args)
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::installNTService", this);
  m_schManager = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS | SC_MANAGER_CREATE_SERVICE);
  if(!m_schManager)
  {
    AFILE_TRACER_DEBUG_MESSAGE("ADaemon::installNTService: Unable to open service manager", this);
    ATHROW_LAST_OS_ERROR(NULL);
  }

  //a_Now that the service manager is open, let's open the service
  char sPath[1024];   //a_Who knows, long filenames may be really long
	GetModuleFileName(GetModuleHandle(NULL), sPath, 1023);

  // "Normalize" the name and add the arguments.
  AString imagePath;
  if ( sPath[0] != '\"' )
  {
    imagePath  = '\"';
    imagePath += sPath;
    imagePath += '\"';
  }
  else
  {
    imagePath = sPath;
  }

  if (!args.isEmpty()) {
    imagePath.append(' ');
    imagePath.append(args);
  }

  m_schService = ::CreateService(
    m_schManager,                      // SC_HANDLE hSCManager,
    m_serviceName.c_str(),             // LPCTSTR lpServiceName,
    m_displayName.c_str(),             // LPCTSTR lpDisplayName,
    SERVICE_ALL_ACCESS,                // DWORD dwDesiredAccess,
    SERVICE_WIN32_OWN_PROCESS,         // DWORD dwServiceType,
    SERVICE_AUTO_START,                // DWORD dwStartType,
    SERVICE_ERROR_NORMAL,              // DWORD dwErrorControl,
    imagePath.c_str(),                 // LPCTSTR lpBinaryPathName,
    NULL,                              // LPCTSTR lpLoadOrderGroup,
    NULL,                              // LPDWORD lpdwTagId,
    NULL,                              // LPCTSTR lpDependencies,
    NULL,                              // LPCTSTR lpServiceStartName,
    NULL                               // LPCTSTR lpPassword
  );

  if (NULL == m_schService)
  {
    AFILE_TRACER_DEBUG_MESSAGE("ADaemon::installNTService: Unable to create service", this);
    u4 lastOSError = ::GetLastError();
    _closeService();
    ATHROW_LAST_OS_ERROR_KNOWN(NULL, lastOSError);
  }
  else
  {
    //a_Set service optional info after create
    if (!m_description.isEmpty())
    {
      SERVICE_DESCRIPTION sd;
      sd.lpDescription = m_description.startUsingCharPtr();
      if (!::ChangeServiceConfig2(
          m_schService,
          SERVICE_CONFIG_DESCRIPTION,
          &sd
        )
      )
        ATHROW_LAST_OS_ERROR(NULL);
    }
  }
  _closeService();

  AFILE_TRACER_DEBUG_MESSAGE("Service installed.", this);

  return 1;
}

int ADaemon::startNTService(int iArgCount, const void **ppcArgValue)
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::startNTService", this);
  try
  {
    _openService();

    if (iArgCount > 0 && ppcArgValue[iArgCount] != NULL)
      ATHROW(NULL, AException::InvalidParameter);    //a_The ppcArgValue array is not NULL terminated

#ifdef UNICODE
    LPCWSTR *argv = (LPCWSTR *)ppcArgValue;
    if (!::StartService(m_schService, iArgCount, argv))
#else
    LPCSTR *argv = (LPCSTR *)ppcArgValue;
    if (!::StartService(m_schService, iArgCount, argv))
#endif
    {
      u4 lastOSError = ::GetLastError();
      if (ERROR_SERVICE_ALREADY_RUNNING == lastOSError)
      {
        AFILE_TRACER_DEBUG_MESSAGE("ADaemon::startNTService: Already running", this);
        return 2;  //a_Not really an error, just a confirmation
      }
      else
      {
        _closeService();
        AFILE_TRACER_DEBUG_MESSAGE((ASWNL("ADaemon::startNTService: Unable to start service, GetLastError()=")+AString::fromU4(lastOSError)).c_str(), this);
        ATHROW_LAST_OS_ERROR_KNOWN(NULL, lastOSError);
      }
    }
  }
  catch(AException& ex)
  {
    AFILE_TRACER_DEBUG_MESSAGE((ASWNL("ADaemon::startNTService: Exception:")+ex.what()).c_str(), this);
    _closeService();
    throw;
  }
  catch(...)
  {
    AFILE_TRACER_DEBUG_MESSAGE("ADaemon::startNTService: Unknown Exception", this);
    _closeService();
    throw;
  }

  _closeService();
  AFILE_TRACER_DEBUG_MESSAGE("ADaemon::startNTService: Service started.", this);

  return 1;
}

int ADaemon::stopNTService()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::stopNTService", this);

  if (thisADaemon->mp_MainServiceThread)
    thisADaemon->mp_MainServiceThread->setRun(false);
  AFILE_TRACER_DEBUG_MESSAGE("ADaemon::stopNTService: Flagged service thread to stop.", this);

  try
  {
    _openService();
    _controlService(SERVICE_CONTROL_STOP);
    _closeService();
  }
  catch (...)
  {
    _closeService();
    throw;
  }

  AFILE_TRACER_DEBUG_MESSAGE("Service stopped.", this);

  return 1;
}

int ADaemon::pauseNTService()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::pauseNTService", this);

  try
  {
    _openService();
    _controlService(SERVICE_CONTROL_PAUSE);
    _closeService();
  }
  catch (...)
  {
    _closeService();
    throw;
  }

  AFILE_TRACER_DEBUG_MESSAGE("Service paused.", this);

  return 1;
}

int ADaemon::continueNTService()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::continueNTService", this);

  try
  {
    _openService();
    _controlService(SERVICE_CONTROL_CONTINUE);
    _closeService();
  }
  catch (...)
  {
    _closeService();
    throw;
  }

  AFILE_TRACER_DEBUG_MESSAGE("Service continued.", this);

  return 1;
}

int ADaemon::removeNTService()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::removeNTService", this);

  if (!existNTService())
    return 1;               //a_If it's not there, removing it is that much simpler

  _openService();           //a_Exception handled by calling process

  SERVICE_STATUS ssNow;
  memset(&ssNow, 0, sizeof(SERVICE_STATUS));
  try
  {
    _getNTServiceStatus(ssNow);

    int iTimeout = 0;
    while (ssNow.dwCurrentState == SERVICE_STOP_PENDING && iTimeout < STOP_TIMEOUT)
    {
      AThread::sleep(1000);
      iTimeout++;
      _getNTServiceStatus(ssNow);
    }

    if (ssNow.dwCurrentState != SERVICE_STOPPED)
    {
      //a_Service is not stopped
      try
      {
        if (!_controlService(SERVICE_CONTROL_STOP))
          _closeService();
      }
      catch(...)
      {
        //a_Failed to stop
        _closeService();
        throw;
      }
    }
  }
  catch(AException &)
  {
    //a_Do nothing, let's try delete, it may nat even be running
  }

  if(!DeleteService(m_schService))
  {
    //a_Unable to delete
    DWORD errorNumber = GetLastError();
    _closeService();
    ATHROW_LAST_OS_ERROR_KNOWN(NULL, errorNumber);
  }

  AFILE_TRACER_DEBUG_MESSAGE("Service removed.", this);

  _closeService();
  return 1;
}

int ADaemon::controlStartPending()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::controlStartPending", this);
  return 1;
}

int ADaemon::controlStopPending()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::controlStopPending", this);
  return 1;
}

int ADaemon::controlInterrogate()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::controlInterrogate", this);
  return 1;
}

int ADaemon::controlShutdown()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::controlShutdown", this);
  return 1;
}

int ADaemon::controlStopped()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::controlStopped", this);
  return 1;
}

int ADaemon::controlContinuePending()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::controlContinuePending", this);
  return 1;
}

int ADaemon::controlRunning()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::controlRunning", this);
  return 1;
}

int ADaemon::controlPausePending()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::controlPausePending", this);
  return 1;
}

int ADaemon::controlPaused()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::controlPaused", this);
  return 1;
}

#endif //__WINDOWS__
