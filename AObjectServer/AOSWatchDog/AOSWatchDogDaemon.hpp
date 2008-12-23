#ifndef INCLUDED__AOSWatchDogDaemon_HPP__
#define INCLUDED__AOSWatchDogDaemon_HPP__

#define ENABLE_AFILE_TRACER_DEBUG                      // This will enable it, comment it out to disable
//#define USE_WINDOWS_DEBUG_OUTPUT                       //OPTIONAL: Instead of a file, redirect output to Windows debug output
#define AFILE_TRACER_FILENAME "C:/tmp/debug.aos.log"   //OPTIONAL: Use another filename

#include "debugFileTracer.hpp"
#include "ADaemon.hpp"
#include "AFilename.hpp"
#include "AINIProfile.hpp"
#include "AHTTPRequestHeader.hpp"

class __declspec(dllexport) AOSWatchDogDaemon : public ADaemon
{
public:
  AOSWatchDogDaemon();
  virtual ~AOSWatchDogDaemon() {}

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
