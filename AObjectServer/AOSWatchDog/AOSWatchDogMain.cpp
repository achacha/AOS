#include "apiABase.hpp"
#include "AOSWatchDogDaemon.hpp"
#include "ASocketLibrary.hpp"
#include "AFile_IOStream.hpp"

#if defined(_MSC_VER)
#pragma comment(lib, "user32")
#endif

ASocketLibrary g_socketLibrary;

static const AString _BUILD_INFO_(ASWNL("AOSWatchDog \tBUILD(")+__TIME__+" "+__DATE__+")");

int main(int argc, char **argv)
{
  std::cout << _BUILD_INFO_ << std::endl;

  if (argc <= 1)
  {
    std::cout << "AOSWatchDog [status | install | start | stop | remove | console]\r\n";
    std::cout << "  install - Installs Windows service.\r\n";
    std::cout << "  start   - Start an already installed service.\r\n";
    std::cout << "  stop    - Stop an already started service.\r\n";
    std::cout << "  remove  - Remove an already installed service.\r\n";
    std::cout << "  console - Run watchdog in console mode (not as a service).\r\n";
  }
  AString strCommand(argv[1]);
  
  static AOSWatchDogDaemon watchdog;
  watchdog.init();
  try
  {
    if (strCommand.equalsNoCase("invoke"))
    {
      if (watchdog.invoke() >= 0)
        std::cout << "Service invoked." << std::endl;
      else
        std::cout << "Service invoke failed." << std::endl;
    }
    else if (strCommand.equalsNoCase("install"))
    {
      if (!watchdog.existNTService())
      {
        if (watchdog.installNTService())
          std::cout << "Service installed." << std::endl;
        else
          std::cout << "Service install failed." << std::endl;
      }
      else
        std::cout << "Service already installed." << std::endl;
    }
    else if (strCommand.equalsNoCase("remove"))
    {
      if (watchdog.removeNTService())
        std::cout << "Service removed." << std::endl;
      else
        std::cout << "Service remove failed." << std::endl;
    }
    else if (strCommand.equalsNoCase("start"))
    {
      if (watchdog.startNTService())
        std::cout << "Service started." << std::endl;
      else
        std::cout << "Service start failed." << std::endl;
    }
    else if (strCommand.equalsNoCase("stop"))
    {
      if (watchdog.stopNTService())
        std::cout << "Service stopped." << std::endl;
      else
        std::cout << "Service stop failed." << std::endl;
    }
    else if (strCommand.equalsNoCase("status"))
    {
      std::cout << watchdog.getStatus() << std::endl;
    }
    else if (strCommand.equalsNoCase("console"))
    {
      AThread thread(AOSWatchDogDaemon::MainServiceThreadProc, true);
      std::cout << "'exit' to exit" << std::endl;
      AFile_IOStream ios;
      AString input;
      while (!input.equalsNoCase("exit"))
      {
        ios.readLine(input);
      }
      thread.setRun(false);
      
      std::cout << "Thread signaled to stop." << std::flush;
      while(thread.isRunning())
      {
        AThread::sleep(100);
        std::cout << "." << std::flush;
      }
    }
    else
      std::cout << "Unknown command: " << strCommand << std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception" << std::endl;
  }
  watchdog.wait();

  return 1;
}
