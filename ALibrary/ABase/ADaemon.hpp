/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED_ADaemon_HPP__
#define INCLUDED_ADaemon_HPP__

#include "AString.hpp"
#include "AThread.hpp"
#include "ASynchronization.hpp"

#if defined(__WINDOWS__)
#define ENABLE_AFILE_TRACER_DEBUG                 // This will enable tracing, comment it out to disable
#define DEBUGFILETRACER_USE_WINDOWS_DEBUG_OUTPUT  // Redirect output to Windows debug output
#endif
#include "debugFileTracer.hpp"

#if defined(SOLARIS)
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <unistd.h>
#endif

#if defined(__WINDOWS__)
extern "C" 
{
  ABASE_API DWORD WINAPI fcbServiceHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

#  ifdef UNICODE
  ABASE_API VOID WINAPI fcbServiceMain(DWORD argc, LPTSTR *argv);
#  else
  ABASE_API VOID WINAPI fcbServiceMain(DWORD argc, LPSTR *argv);
#  endif
}

#endif

class ABASE_API ADaemon
{
public:
  /*!
  UNIX and NT
  */
  ADaemon(const AString& serviceName, const AString& displayName, const AString& description);
  virtual ~ADaemon();

  // This one throws no exceptions, they are caught inside and a simple value is returned (non-zero is successful).
  // dwParam is passed to the UNIX child process
  virtual int invoke(u4 dwParam = 0, char **ppcParam = NULL);

  /*!
  Threadproc of the service
  */
  virtual u4 callbackMain(AThread& thread) = 0;

  // Must be called as a first thing inside the main() function (does nothing under UNIX)
  void init();                                //a_Initialize the NTService
  void wait();                                //a_Wait for service thread to exit (performs a thread join)

  //! Calls the virtual callbackMain
  static u4 MainServiceThreadProc(AThread& thread);

#if defined(__WINDOWS__)
  SERVICE_STATUS_HANDLE sm_sshService;  //a_Set once the service is registered

  // NT only: Service informative
  int existNTService();                                     //a_Checks if this service exists in system database
  AString getStatus();                                      //a_Returns textual status of the service's state
  int getNTServiceStatus(SERVICE_STATUS &);                 //a_Wrapper for the protected function, doesn't throw an exception
  int isRunningInServiceMode() { return m_iServiceMode; }  //a_non-zero if service manager is running this, else must be console mode

  // NT only: Service life-dependent
  int installNTService(const AString &args = AConstant::ASTRING_EMPTY);          //a_Installs the current service
  int removeNTService();                                                    //a_Removes the current service
  
  /*!
  Start a service and optionally pass parameters

  If UNICODE then ppcArgValue is LPCWSTR*
  Else ppcArgValue is LPCSTR*
  */
  int startNTService(int iArgCount = 0, const void **ppcArgValue = NULL);

  // NT only: Service controlling
  int pauseNTService();     //a_Pauses the current service
  int continueNTService();  //a_Continues the current service (given it was paused, else it will hurl ye holy exception of the antiochs at thee)
  int stopNTService();      //a_Stops the current service

  // NT service notifcation
  int notifyServiceControlManager(u4 dwState, u4 dwProgress);

  static AThread *mp_MainServiceThread;

  /*!
  service name - internal name of this service (unique and no spaces, not visible to user)
  display name - name of the service as show to the user
  description - description of the service, visible to the user (2nd column in cservic manager cpl)
  */
  //a_Defined by user of the class and instane specific
  AString m_serviceName;
  AString m_displayName;
  AString m_description;

  /*!
  Handlers for service state changes
  */
  virtual int controlStartPending();
  virtual int controlStopPending();
  virtual int controlInterrogate();
  virtual int controlShutdown();
  virtual int controlStopped();
  virtual int controlContinuePending();
  virtual int controlRunning();
  virtual int controlPausePending();
  virtual int controlPaused();
#else
  virtual int controlStartPending();
#endif

protected:
#if defined(__WINDOWS__)
  // These are the workhorses for service control
  int _openService();                          //a_throws Exception
  int _getNTServiceStatus(SERVICE_STATUS &);   //a_throws Exception
  int _closeService();
  int _isOpenService() { return (m_schManager && m_schService); }  //a_Inlined for speed
  int _connectNTServiceToDispatcher();             //a_Checks if this object is running as a service process
  int _controlService(u4 dwParam);                 //a_throws Exception

  int m_iServiceMode;               //a_If running in service mode
  
  SERVICE_TABLE_ENTRY m_steServiceTable[2];         //a_Service entry table used when opening and accessing the service
  SC_HANDLE           m_schService,                 //a_Service handle
                      m_schManager;                 //a_ServiceControl manager handle
#else
  //a_UNIX only
  static void fcbServiceMain(u4 dwArgc, char **lpszArgv);

  pid_t               m_pidParent,                  //a_Process ID for parent
                      m_pidChild;                   //a_Process ID for child
#endif

private:
  ADaemon() {}
};

// There is 1 instance of a service per executable set when ADaemon object is created
extern ADaemon *thisADaemon;


#endif  //#ifndef INCLUDED_ADaemon_HPP__

/////////////////////////////////////////////////////////////////////////////////
//
// ADaemon - class used for summoning and banishing daemon processes (ie. services)
// 
// If you get error 1053 instantly when starting a service, something is not being exported or function is not being found.
//   Remember __declspec(dllexport) for your derived class so it is visible to service manager.
//
//  Example service
//--------------------------------------------START CUT CPP---------------------------------------
//#include "ADaemon.hpp"
//#include "AException.hpp"
//#include "AFile_IOStream.hpp"
//
//#ifdef __WINDOWS__
//class __declspec(dllexport) MyService : public ADaemon
//#else
//class MyService : public ADaemon
//#endif
//{
//public:
//  MyService() : ADaemon("MyServiceTest", "MyServiceDisplay", "Description of my service.") {}
//  virtual ~MyService() {}
//
//  //NOTE: It is critical that you do NOT exit this function until thread.isRun() == false
//  // A service should run until it is asked to stop, if this is a one-shot process,
//  //  then use a sleep look and wait for the tread to be signaled to stop
//  virtual u4 callbackMain(AThread& thread)
//  {
//    while (thread.isRun())
//      AThread::sleep(1000);
//
//    return 0;
//  }
//};
//
//int main(int argc, char **argv)
//{
//  if (argc < 2)
//  {
//    std::cout << "Usage: this <install|start|stop|remove|console>" << std::endl;
//  }
//
//  static MyService service;
//  service.init();
//  try
//  {
//    AString command(argv[1]);
//    if (command.equalsNoCase("install"))
//    {
//      service.installNTService();
//    }
//    else if (command.equalsNoCase("remove"))
//    {
//      service.removeNTService();
//    }
//    else if (command.equalsNoCase("start"))
//    {
//      service.startNTService();
//    }
//    else if (command.equalsNoCase("stop"))
//    {
//      service.stopNTService();
//    }
//    else if (command.equalsNoCase("console"))
//    {
//      //a_Start a thread with main service proc and read keyboard input to exit
//      AThread thread(service.MainServiceThreadProc, true);
//      std::cout << "Type 'exit' and Enter, to signal service thread to stop." << std::endl;
//      AFile_IOStream ios;
//      AString input;
//      while (!input.equalsNoCase("exit"))
//      {
//        ios.readLine(input);
//      }
//      thread.setRun(false);  //a_Flag thread to stop execution
//      std::cout << "Thread signaled to stop." << std::flush;
//      
//      //a_Wait until it exits gracefully
//      while(thread.isRunning())
//      {
//        AThread::sleep(100);
//        std::cout << "." << std::flush;
//      }
//      std::cout << "Bye." << std::flush;
//    }
//    else
//      std::cerr << "Unknown command: " << command <<std::endl;
//  }
//  catch(AException& ex)
//  {
//    std::cerr << ex.what() << std::endl;
//  }
//  catch(...)
//  {
//    std::cerr << "Unknown exception" << std::endl;
//  }
//  service.wait();
//
//  return 0;
//}
//--------------------------------------------END CUT CPP---------------------------------------
//  NT service callback function information (Buttons in ControlPanel::Services dialog)
//  Override any that you need, default behaviour is to silently continue and do nothing
//  START    - controlStartPending()           - returning 0 will abort the START
//             controlRunning()                - service was successfully started
//  STOP     - controlStopPending()            - returning 0 will abort the STOP
//           - controlStopped()                - service was successfully stopped
//  PAUSE    - controlPausePending()           - returning 0 will abort the PAUSE
//           - controlPaused()                 - service was successfully paused
//  CONTINUE - controlContinuePending()        - returning 0 will abort the CONTINUE
//           - controlRunning()                - service was successfully started/resumed
//
//  Internal INTERROGATE - controlInterrogate  - returning 0 will not update the control manager
//
//
//******************************************************************************
