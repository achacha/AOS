#include "apiABase.hpp"
#if defined(__WINDOWS__)
#define ENABLE_AFILE_TRACER_DEBUG                 // This will enable tracing, comment it out to disable
#define DEBUGFILETRACER_USE_WINDOWS_DEBUG_OUTPUT  // Redirect output to Windows debug output
#endif
#include "debugFileTracer.hpp"
#include "AThread.hpp"
#include "ASystemException.hpp"

#define AOS_SERVICE_NAME TEXT("AOSRhinoWatchDog")
#define AOS_SERVICE_DESC TEXT("AOS Rhino WatchDog")

__declspec(dllexport) DWORD WINAPI aosServiceHandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
  AFILE_TRACER_DEBUG_SCOPE("aosServiceHandler", NULL);
  return NO_ERROR;
}  

__declspec(dllexport) VOID WINAPI aosServiceMain(DWORD argc, LPSTR *argv)
{
  AFILE_TRACER_DEBUG_SCOPE("aosServiceMain", NULL);

  SERVICE_STATUS_HANDLE sshService = ::RegisterServiceCtrlHandlerEx(TEXT("AOSRhinoWatchDog"), (LPHANDLER_FUNCTION_EX)aosServiceHandlerEx, NULL);
  if(!sshService)
  {
    AFILE_TRACER_DEBUG_MESSAGE("aosServiceMain failed to register service handler.", NULL);
    return;
  }
  AFILE_TRACER_DEBUG_MESSAGE("aosServiceMain: Handler registered", NULL);

  return;
}

void closeService(SC_HANDLE schManager, SC_HANDLE schService)
{
  if (schService)
  {
    ::CloseServiceHandle(schService);     //a_Close the service
  }
  if (schManager)
  {
    ::CloseServiceHandle(schManager);     //a_Close the manager
  }
}

bool installService()
{
  AFILE_TRACER_DEBUG_SCOPE("installService", NULL);
  
  AFILE_TRACER_DEBUG_MESSAGE("Service connected to controller.", NULL);
  SC_HANDLE schManager = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS | SC_MANAGER_CREATE_SERVICE);
  if(!schManager)
  {
    AFILE_TRACER_DEBUG_MESSAGE("Unable to open service manager", NULL);
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

  SC_HANDLE schService = ::CreateService(
    schManager,                        // SC_HANDLE hSCManager,      
    AOS_SERVICE_NAME,                  // LPCSTR lpServiceName,     
    AOS_SERVICE_NAME,                  // LPCSTR lpDisplayName,     
    SERVICE_ALL_ACCESS,                // DWORD dwDesiredAccess,     
    SERVICE_WIN32_OWN_PROCESS,         // DWORD dwServiceType,       
    SERVICE_AUTO_START,                // DWORD dwStartType,         
    SERVICE_ERROR_NORMAL,              // DWORD dwErrorControl,      
    TEXT(imagePath.c_str()),           // LPCSTR lpBinaryPathName,  
    NULL,                              // LPCSTR lpLoadOrderGroup,  
    NULL,                              // LPDWORD lpdwTagId,         
    NULL,                              // LPCSTR lpDependencies,    
    NULL,                              // LPCSTR lpServiceStartName,
    NULL                               // LPCSTR lpPassword         
  );

  if (NULL == schService)
  {
    AFILE_TRACER_DEBUG_MESSAGE("Unable to create service", NULL);
    u4 lastOSError = ::GetLastError();
    closeService(schManager, schService);
    ATHROW_LAST_OS_ERROR_KNOWN(NULL, lastOSError);
  }
  else
  {
    //a_Set service optional info after create
    SERVICE_DESCRIPTION sd;
    sd.lpDescription = AOS_SERVICE_DESC;
    if (!::ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &sd))
      ATHROW_LAST_OS_ERROR(NULL);
  }
  closeService(schManager, schService);
  AFILE_TRACER_DEBUG_MESSAGE("Service installed.", NULL);

  return true;
}

bool removeService()
{
  AFILE_TRACER_DEBUG_SCOPE("removeService", NULL);
  SC_HANDLE schManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (NULL == schManager)
  {
    AFILE_TRACER_DEBUG_MESSAGE("Unable to open service control manager", NULL);
    ATHROW_LAST_OS_ERROR(NULL);
  }

  AFILE_TRACER_DEBUG_MESSAGE((AString("Trying to open service: ")+AOS_SERVICE_NAME).c_str(), NULL);
  SC_HANDLE schService = ::OpenService(schManager, AOS_SERVICE_NAME, SERVICE_ALL_ACCESS);
  if (NULL == schService)
  {
    //a_Open service failed
    AFILE_TRACER_DEBUG_MESSAGE("Unable to open service", NULL);
    u4 lastOSError = ::GetLastError();
    closeService(schManager, schService);
    ATHROW_LAST_OS_ERROR_KNOWN_EX(NULL, lastOSError, AString("Unable to open service: ")+AOS_SERVICE_NAME);
  }
  AFILE_TRACER_DEBUG_MESSAGE((ASWNL("Service opened: ")+AOS_SERVICE_NAME).c_str(), NULL);

  if(schService && !DeleteService(schService))
  {
    //a_Unable to delete
    DWORD errorNumber = GetLastError();
    closeService(schManager, schService);
    ATHROW_LAST_OS_ERROR_KNOWN(NULL, errorNumber);
  }

  AFILE_TRACER_DEBUG_MESSAGE("Service removed.", NULL);
  closeService(schManager, schService);
  return 1;
}

void connectToServiceDispatch()
{
  AFILE_TRACER_DEBUG_SCOPE("connectToServiceDispatch", NULL);
  SERVICE_TABLE_ENTRY steServiceTable[2];
  steServiceTable[0].lpServiceName = AOS_SERVICE_NAME;
  steServiceTable[0].lpServiceProc = aosServiceMain;
  steServiceTable[1].lpServiceName = NULL;         //a_Must be NULL
  steServiceTable[1].lpServiceProc = NULL;         //a_Must be NULL

  //a_Connect to service controller using the current service table
  AFILE_TRACER_DEBUG_MESSAGE((ASWNL("Calling StartServiceCtrlDispatcher: ")+AOS_SERVICE_NAME).c_str(), NULL);
  if(!::StartServiceCtrlDispatcher(steServiceTable))
  {
    switch(::GetLastError())
    {
      case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT :
        AFILE_TRACER_DEBUG_MESSAGE("WARNING: console mode", NULL);

      case ERROR_INVALID_DATA :
        AFILE_TRACER_DEBUG_MESSAGE("Invalid data", NULL);
        ATHROW(NULL, AException::InvalidData);

      default :
        AFILE_TRACER_DEBUG_MESSAGE("Unknown error", NULL);
    }
  }
  AFILE_TRACER_DEBUG_MESSAGE("Service dispatcher started", NULL);
}

int main(int argc, char **argv)
{
  AString strCommand(argv[1]);
  try
  {
    if (strCommand.equalsNoCase("install"))
    {
      if (installService())
        std::cout << "Service installed." << std::endl;
      else
        std::cout << "Service install failed." << std::endl;
    }
    else if (strCommand.equalsNoCase("remove"))
    {
      if (removeService())
        std::cout << "Service removed." << std::endl;
      else
        std::cout << "Service remove failed." << std::endl;
    }
    else
    {
      // When service dispatch action is requested this main() is called with no parameters
      // This connects the handler with service
      connectToServiceDispatch();
    }
  }
  catch(AException& ex)
  {
    AFILE_TRACER_DEBUG_MESSAGE((AString("main: ")+ex.what()).c_str(), NULL);
    std::cerr << ex.what() << std::endl;
  }
  catch(std::exception& ex)
  {
    AFILE_TRACER_DEBUG_MESSAGE((AString("main: ")+ex.what()).c_str(), NULL);
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    AFILE_TRACER_DEBUG_MESSAGE("main: Unknown exception", NULL);
    std::cerr << "Unknown exception" << std::endl;
  }

  return 1;
}