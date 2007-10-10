#include "pchABase.hpp"
#include "ADaemon.hpp"
#include "ASystemException.hpp"
#include "ALock.hpp"

ADaemon *ADaemon::mp_This(NULL);
AThread *ADaemon::smp_MainServiceThread(NULL);  //a_Main service thread

#ifdef __WINDOWS__
#pragma message("Compiling Windows service code.")

int ADaemon::sm_iTIMEOUT = 6;            //a_6 seconds timeout (eternity for a computer sometimes)

DWORD WINAPI ADaemon::fcbServiceHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::fcbServiceHandler", NULL);
  
  mp_This->init();
  SERVICE_STATUS ssNow;
  if (!mp_This->getNTServiceStatus(ssNow))
    return NO_ERROR;     //a_Problem getting status

  try
  {
    switch(dwControl)
    {
      case SERVICE_CONTROL_INTERROGATE:
      {
        if (mp_This->controlInterrogate())
         mp_This->notifyServiceControlManager(ssNow.dwCurrentState, 0);
      }
      break;

      case SERVICE_CONTROL_STOP:
        smp_MainServiceThread->setRun(false);
        AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:STOP: Flagging thread to stop", NULL);
        mp_This->notifyServiceControlManager(SERVICE_STOP_PENDING, 1);
        if (mp_This->controlStopPending() == 0)
        {
          //a_Abort requested
          mp_This->notifyServiceControlManager(ssNow.dwCurrentState, 1);
          return NO_ERROR;
        }
        
        {
          const int iiTimeout = 10;
          int i=0;
          while (smp_MainServiceThread->isRunning() && ++i < iiTimeout)
          {
            AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler: Waiting for service thread to stop", NULL);
            AThread::sleep(1000);
          }

          if (i >= iiTimeout)
          {  
            AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:STOP: Sledgehammer to the thread, timeout detected", NULL);
            smp_MainServiceThread->terminate();       //a_Sledgehammer after timeout
          }
        }

        mp_This->notifyServiceControlManager(SERVICE_STOPPED, 0);
        mp_This->controlStopped();
        AFILE_TRACER_DEBUG_MESSAGE("fcbServiceHandler:STOP: Stopped", NULL);
     break;

      case SERVICE_CONTROL_SHUTDOWN:
        mp_This->controlShutdown();
      break;

      case SERVICE_CONTROL_PAUSE:
        mp_This->notifyServiceControlManager(SERVICE_PAUSE_PENDING, 1);
        if (mp_This->controlPausePending() == 0)
        {
          //a_Abort requested
          mp_This->notifyServiceControlManager(ssNow.dwCurrentState, 1);
          return NO_ERROR;
        }
      
        if (smp_MainServiceThread)
          smp_MainServiceThread->suspend();
        mp_This->notifyServiceControlManager(SERVICE_PAUSED, 0);
        mp_This->controlPaused();
      break;

      case SERVICE_CONTROL_CONTINUE:
        if (smp_MainServiceThread)
          smp_MainServiceThread->resume();
        mp_This->notifyServiceControlManager(SERVICE_CONTINUE_PENDING, 1);
        if (mp_This->controlContinuePending() == 0)
        {
          //a_Abort requested
          mp_This->notifyServiceControlManager(ssNow.dwCurrentState, 1);
          return NO_ERROR;
        }
      
        mp_This->notifyServiceControlManager(SERVICE_RUNNING, 0);
        mp_This->controlRunning();
      break;
    }
  }
  catch(AException &eX)
  {
    MessageBox(NULL,eX.what().toAString().c_str(),"ADaemon::fcbServiceHandler",MB_SERVICE_NOTIFICATION);
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
VOID WINAPI ADaemon::fcbServiceMain(DWORD argc, LPTSTR *argv)
{
  if (!mp_This)
  {
    MessageBox(NULL,"mp_This is NULL, this means service class was never created in this instance or went out of scope, should be static or global","ADaemon::fcbServiceMain",MB_SERVICE_NOTIFICATION);
    return;
  }

  if(!(mp_This->sm_sshService = ::RegisterServiceCtrlHandlerEx(mp_This->m_serviceName.c_str(), (LPHANDLER_FUNCTION_EX)fcbServiceHandler, NULL)))
  {
    AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain failed to register service handler.", NULL);
    return;
  }
  AFILE_TRACER_DEBUG_SCOPE("fcbServiceMain", NULL);
  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Handler registered", NULL);

  //a_This will make the litle clock go by in the dialog
  mp_This->notifyServiceControlManager(SERVICE_START_PENDING, 1);
  
  //a_Start pending, start-up code should go here
  if (mp_This->controlStartPending() == 0)
    return;                           //a_Abort requested
 
  //a_Update the clock display if needed
  mp_This->notifyServiceControlManager(SERVICE_START_PENDING, 3);

  //a_Create a new thread for the service
  try 
  {
    delete smp_MainServiceThread;      //a_This should always just return, as it should point to NULL
    smp_MainServiceThread = new AThread((AThread::ATHREAD_PROC *)MainServiceThreadProc, false);
    AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Service thread created", NULL);
  }
  catch(AException &eX)
  {
    AString& str = eX.what().toAString();
    AFILE_TRACER_DEBUG_MESSAGE(str.c_str(), NULL);
    MessageBox(NULL,str.c_str(),"ADaemon::fcbServiceMain",MB_SERVICE_NOTIFICATION);
    return;
  }
  catch(...)
  {
    MessageBox(NULL,"Unknown Exception Caught","ADaemon::fcbServiceMain",MB_SERVICE_NOTIFICATION);
    return;
  }
  
  mp_This->notifyServiceControlManager(SERVICE_START_PENDING, 4);
  smp_MainServiceThread->setRun(true);  //a_Set thread to Run, the thread will in turn set Running flag to true when it is actually running
  smp_MainServiceThread->start();

  u4 progress = 5;
  while (smp_MainServiceThread->isRun() && smp_MainServiceThread->isRunning())
  {
    AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Service waiting for thread to start", NULL);
    AThread::sleep(500);
    mp_This->notifyServiceControlManager(SERVICE_START_PENDING, progress++);
  }

  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Service thread started", NULL);
  mp_This->notifyServiceControlManager(SERVICE_RUNNING, 0);
  mp_This->controlRunning();

  //a_Wait until done
  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain: Starting to wait for signal of termination.", NULL);

  //a_Wait until main thread finishes
  do
  {
    AThread::sleep(2000);
  }
  while(smp_MainServiceThread->isRunning());
    

  //a_Finish the thread and delete
  AFILE_TRACER_DEBUG_MESSAGE((AString("fcbServiceMain: Thread stop signaled, terminating thread: ") + AString::fromPointer(smp_MainServiceThread)).c_str(), NULL);    

  if (smp_MainServiceThread->isRunning())
    smp_MainServiceThread->terminate();

  delete smp_MainServiceThread;
  smp_MainServiceThread = NULL;

  return;
}
#else

void fcbServiceMain(u4 dwArgc, char **lpszArgv)
{
  //a_UNIX (sun) code for the service
  //a_Start pending, start-up code should go here
  AFILE_TRACER_DEBUG_SCOPE("fcbServiceMain", NULL);

  if (controlStartPending() == 0)
    return;                           //a_Abort requested

  //a_Create a new thread for the service
  try 
  {
    delete smp_MainServiceThread;      //a_This should always just return, as it should point to NULL
    smp_MainServiceThread = new AThread((AThread::ATHREAD_PROC *) fcbMainServiceThread, true);
  }
  catch(AException &eX)
  {
    AFILE_TRACER_DEBUG_MESSAGE(eX.what(), NULL);
    return;
  }
  
  smp_MainServiceThread->setRun();

  //a_Wait until done
  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain::Starting to wait for signal of termination.", NULL);
  while(smp_MainServiceThread->isRunning())
    AThread::sleep(2000);

  AFILE_TRACER_DEBUG_MESSAGE("fcbServiceMain::Exiting!", NULL);

  if (smp_MainServiceThread->isRunning())
  {
  //a_Finish the thread and delete
    smp_MainServiceThread->terminate();
    AFILE_TRACER_DEBUG_MESSAGE((AString("fcbServiceMain::Terminating thread: ") + AString::fromPointer(smp_MainServiceThread)).c_str(), NULL);    
  }
  delete smp_MainServiceThread;
  smp_MainServiceThread = NULL;

  return;
}

#endif

u4 ADaemon::MainServiceThreadProc(AThread& thread)
{
  u4 ret = 0;
  if (!mp_This)
  {
    MessageBox(NULL,"ADaemon global object does not exist, callbackMain not being called.","ADaemon::MainServiceThreadProc",MB_SERVICE_NOTIFICATION);
    return -1;
  }
  else
  {
    thread.setRunning(true);
    AFILE_TRACER_DEBUG_SCOPE("MainServiceThreadProc: ADaemon::callbackMain", NULL);    
    ret = mp_This->callbackMain(thread);
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
  m_steServiceTable[0].lpServiceProc = fcbServiceMain;
  m_steServiceTable[1].lpServiceName = NULL;         //a_Must be NULL
  m_steServiceTable[1].lpServiceProc = NULL;         //a_Must be NULL

  m_schService = NULL;                               //a_ServiceControl service
  m_schManager = NULL;                               //a_ServiceControl manager  
  sm_sshService = NULL;

  //a_Connects this service to a service dispatcher if it exists
  m_iServiceMode = 0;
  
  AASSERT(NULL, !mp_This);      //a_Only one instance per process
  mp_This = this;

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

  mp_This = this;
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
    ATHROW_EX(this, AException, AException::OperationFailed, "ADaemon::invokeUnable to fork process.");
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

  if (m_pdcbMain)
    (*m_pdcbMain)(dwParam, ppcParam);   //a_Call the main process
  else
    ATHROW_EX(this, AException, AException::OperationFailed, "ADaemon::invoke:Main process missing.");

  AFILE_TRACER_DEBUG_MESSAGE("ADaemon::invoke: returning", NULL);

  return m_pidChild;                    //a_The child finished
#endif

}

void ADaemon::init()
{
#if defined(__WINDOWS__)
  _connectNTServiceToDispatcher();
#endif
}

void ADaemon::wait()
{
#if defined(__WINDOWS__)
  if (smp_MainServiceThread)
    smp_MainServiceThread->waitForThreadToExit();
#endif
}

#if defined(__WINDOWS__)
int ADaemon::getNTServiceStatus(SERVICE_STATUS &ssNow)
{
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
  //a_Open the service if needed
  SERVICE_STATUS ssMe;
  memset(&ssMe, 0, sizeof(SERVICE_STATUS));
  if (_getNTServiceStatus(ssMe) > 0)   //a_This will throw an exception to be caugth by caller of this function
  {
    //a_Got the structure filled
    if (!ControlService(m_schService, dwParam, &ssMe))
    {
      u4 lastOSError = ::GetLastError();
      _closeService();
      
      //a_Do not throw, just use exception to look up error for logging
      ASystemException osx(lastOSError, NULL, __FILE__, __LINE__);
      AFILE_TRACER_DEBUG_MESSAGE((ASWNL("ADaemon::_controlService: Unable to control service: ")+osx.what()).c_str(), this);
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

int ADaemon::notifyServiceControlManager(u4 dwState, u4 dwProgress)
{
  AFILE_TRACER_DEBUG_SCOPE("notifyServiceControlManager", NULL);

  SERVICE_STATUS ssX;
  
  ssX.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  ssX.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
  ssX.dwServiceSpecificExitCode = 0;
  ssX.dwCurrentState = dwState;
  ssX.dwWin32ExitCode = 0;
  ssX.dwCheckPoint = dwProgress;
  ssX.dwWaitHint = 5000; // wait for state change

  return ::SetServiceStatus(sm_sshService, &ssX);
}

int ADaemon::_connectNTServiceToDispatcher()
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::_connectNTServiceToDispatcher", this);

  //a_Connect to service controller using the current service table
  AFILE_TRACER_DEBUG_MESSAGE((ASWNL("ADaemon::_connectNTServiceToDispatcher: Calling StartServiceCtrlDispatcher: ")+m_steServiceTable[0].lpServiceName).c_str(), this);
  if(!::StartServiceCtrlDispatcher(m_steServiceTable))
  {
    switch(::GetLastError())
    {
      case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT :
        AFILE_TRACER_DEBUG_MESSAGE("ADaemon::_connectNTServiceToDispatcher: console mode", this);
        m_iServiceMode = 0; 
      return 0;                   //a_Could not connect with controller, console mode

      case ERROR_INVALID_DATA :
        AFILE_TRACER_DEBUG_MESSAGE("ADaemon::_connectNTServiceToDispatcher: Invalid data", this);
        ATHROW(NULL, AException::InvalidData);

      default :
        AFILE_TRACER_DEBUG_MESSAGE("ADaemon::_connectNTServiceToDispatcher: Unknown error", this);
        return -1;    //a_Some other unknown error?!?
    }
  }
  
  m_iServiceMode = 1;           //a_Service mode

  AFILE_TRACER_DEBUG_MESSAGE("Service connected to controller.", this);

  return 1;
}

int ADaemon::installNTService(const AString &args)
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::installNTService", this);
  m_schManager = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS | SC_MANAGER_CREATE_SERVICE);
  if(NULL == m_schManager)
  {
    AFILE_TRACER_DEBUG_MESSAGE("ADaemon::installNTService: Unable to open service manager", this);
    ATHROW_LAST_OS_ERROR(NULL);
  }

  //a_Now that the service manager is open, let's open the service
  char sPath[1024];   //a_Who knows, long filenames may be really long
	GetModuleFileName(GetModuleHandle(NULL), sPath, 1023);

  // "Normalize" the name and add the arguments.
  AString imagePath;
  if ( sPath[0] != '\"' ) {
    imagePath  = '\"';
    imagePath += sPath;
    imagePath += '\"';
  } else {
    imagePath = sPath;
  }

  if (!args.isEmpty()) {
    imagePath.append(' ');
    imagePath.append(args);
  }

  ServiceDatabaseSynch synchObject(m_schManager);
  ALock lock(synchObject);
  m_schService = ::CreateService(
    m_schManager,                     // SC_HANDLE hSCManager,      
    m_serviceName.c_str(),              // LPCTSTR lpServiceName,     
    m_displayName.c_str(),              // LPCTSTR lpDisplayName,     
    SERVICE_ALL_ACCESS,                // DWORD dwDesiredAccess,     
    SERVICE_WIN32_OWN_PROCESS
      | SERVICE_INTERACTIVE_PROCESS
      ,                                // DWORD dwServiceType,       
    SERVICE_DEMAND_START,              // DWORD dwStartType,         
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

int ADaemon::startNTService(int iArgCount, const char **ppcArgValue)
{
  AFILE_TRACER_DEBUG_SCOPE("ADaemon::startNTService", this);
  try
  {
    _openService();

    if (iArgCount > 0 && ppcArgValue[iArgCount] != NULL)
      ATHROW(NULL, AException::InvalidParameter);    //a_The ppcArgValue array is not NULL terminated
    
    if (!::StartService(m_schService, iArgCount, ppcArgValue))
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

  if (smp_MainServiceThread)
    smp_MainServiceThread->setRun(false);
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
  if (!existNTService())
    return 1;               //a_If it's not there, removing it is that much simpler

  _openService();           //a_Exception handled by calling process
  
  SERVICE_STATUS ssNow;
  memset(&ssNow, 0, sizeof(SERVICE_STATUS));
  try
  {
    _getNTServiceStatus(ssNow);

    int iTimeout = 0;
    while (ssNow.dwCurrentState == SERVICE_STOP_PENDING && iTimeout < sm_iTIMEOUT)
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
        _controlService(SERVICE_CONTROL_STOP);
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
    _closeService();
    ATHROW_LAST_OS_ERROR(NULL);
  }

  AFILE_TRACER_DEBUG_MESSAGE("Service removed.", this);

  _closeService();
  return 1;
}

ADaemon::ServiceDatabaseSynch::ServiceDatabaseSynch(
  SC_HANDLE hSCManager, 
  eInitialState i // = ASynchronization::UNLOCKED
) :
  m_hSCManager(hSCManager),
  m_scLock(NULL)
{
  if (i == ASynchronization::LOCKED)
    lock();
}

ADaemon::ServiceDatabaseSynch::~ServiceDatabaseSynch()
{
  if (m_scLock)
    unlock();
}

void ADaemon::ServiceDatabaseSynch::lock()
{
  AASSERT(NULL, !m_scLock);
  AASSERT(NULL, m_hSCManager);
  if(!islocked())
  {
    m_scLock = LockServiceDatabase(m_hSCManager);
  }
}

bool ADaemon::ServiceDatabaseSynch::islocked()
{
  AASSERT(NULL, m_hSCManager);

  QUERY_SERVICE_LOCK_STATUS qslStatus;
  DWORD dwBytesNeeded;
  if(!QueryServiceLockStatus(m_hSCManager,&qslStatus,sizeof(QUERY_SERVICE_LOCK_STATUS),&dwBytesNeeded))
    return (qslStatus.fIsLocked ? true : false);
  else
    ATHROW_LAST_OS_ERROR(NULL);
}

bool ADaemon::ServiceDatabaseSynch::trylock()
{
  AASSERT(NULL, !m_scLock);
  AASSERT(NULL, m_hSCManager);
  if(!islocked())
  {
    m_scLock = LockServiceDatabase(m_hSCManager);
    return true;
  }
  else
    return false;
}

void ADaemon::ServiceDatabaseSynch::unlock()
{
  AASSERT(NULL, m_scLock);
  UnlockServiceDatabase(m_scLock);
  m_scLock = NULL;
}

int ADaemon::controlStartPending()
{
  return 1;
}

int ADaemon::controlStopPending()
{
  return 1;
}

int ADaemon::controlInterrogate()
{
  return 1;
}

int ADaemon::controlShutdown()
{
  return 1;
}

int ADaemon::controlStopped()
{
  return 1;
}

int ADaemon::controlContinuePending()
{
  return 1;
}

int ADaemon::controlRunning()
{
  return 1;
}

int ADaemon::controlPausePending()
{
  return 1;
}

int ADaemon::controlPaused()
{
  return 1;
}
#else  //__WINDOWS__
int ADaemon::controlStartPending()
{
  return 1;
}

#endif //__WINDOWS__
