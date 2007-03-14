#ifndef INCLUDED__AOSWatchDogDaemon_HPP__
#define INCLUDED__AOSWatchDogDaemon_HPP__

#define ENABLE_AFILE_TRACER_DEBUG                      // This will enable it, comment it out to disable
#ifdef __DEBUG_DUMP__
#define USE_WINDOWS_DEBUG_OUTPUT                       //OPTIONAL: Instead of a file, redirect output to Windows debug output
#endif
#include "debugFileTracer.hpp"
#include "ADaemon.hpp"
#include "AFilename.hpp"

class __declspec(dllexport) AOSWatchDogDaemon : public ADaemon
{
public:
  AOSWatchDogDaemon();
  virtual ~AOSWatchDogDaemon() {}

  virtual u4 callbackMain(AThread& thread);

private:
  bool _startAObjectServer();

  AFilename m_aosExecutablePath;

  PROCESS_INFORMATION m_ProcessInformation;
  STARTUPINFO m_StartupInfo;
};

#endif //INCLUDED__AOSWatchDogDaemon_HPP__
