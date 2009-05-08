#ifndef INCLUDED__AOSWatchDogDaemon_HPP__
#define INCLUDED__AOSWatchDogDaemon_HPP__

#define ENABLE_AFILE_TRACER_DEBUG                    // This will enable it, comment it out to disable
#define DEBUGFILETRACER_USE_WINDOWS_DEBUG_OUTPUT     // Redirect output to Windows debug output

#include "apiABase.hpp"
#include "ADaemon.hpp"

#include "AFilename.hpp"
#include "AINIProfile.hpp"
#include "AHTTPRequestHeader.hpp"

class __declspec(dllexport) AOSWatchDogDaemon : public ADaemon
{
public:
  AOSWatchDogDaemon();
  virtual ~AOSWatchDogDaemon() {}

  //! Main callback for the service thread
  virtual u4 callbackMain(AThread& thread);

  //! Callbacks
  virtual int controlStopPending();

  //! Utility to bounce server
  bool bounceServer();

  //! Start server
  bool startServer();
  
  //! Stop server
  bool stopServer();

private:
  bool _init();
  bool _terminateAObjectServer();
  
  u4 m_SleepTime;
  u4 m_SleepCycles;

  /*
  <0 critical error
  0 no alive
  1 alive
  2 alive but in error state
  */
  int _isAObjectServerAlive();

  AINIProfile m_ini;
  AFilename m_aosExecutablePath;
  AHTTPRequestHeader m_RequestHeader;
  AHTTPRequestHeader m_AdminRequestHeader;

  PROCESS_INFORMATION m_ProcessInformation;
  STARTUPINFO m_StartupInfo;
};

#endif //INCLUDED__AOSWatchDogDaemon_HPP__
