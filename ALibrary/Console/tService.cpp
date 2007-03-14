#include "ADaemon.hpp"
#include "AException.hpp"
#include "AFile_IOStream.hpp"

#ifdef __WINDOWS__
class __declspec(dllexport) MyService : public ADaemon
#else
class MyService : public ADaemon
#endif
{
public:
  MyService() : ADaemon("MyServiceTest", "MyServiceDisplay", "Description of my service.") {}
  virtual ~MyService() {}

  //NOTE: It is critical that you do NOT exit this function until thread.isRun() == false
  // A service should run until it is asked to stop, if this is a one-shot process,
  //  then use a sleep look and wait for the tread to be signaled to stop
  virtual u4 callbackMain(AThread& thread)
  {
    while (thread.isRun())
      AThread::sleep(1000);

    return 0;
  }
};

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cout << "Usage: this <install|start|stop|remove|console>" << std::endl;
  }

  static MyService service;
  service.init();
  try
  {
    AString command(argv[1]);
    if (command.equalsNoCase("install"))
    {
      service.installNTService();
    }
    else if (command.equalsNoCase("remove"))
    {
      service.removeNTService();
    }
    else if (command.equalsNoCase("start"))
    {
      service.startNTService();
    }
    else if (command.equalsNoCase("stop"))
    {
      service.stopNTService();
    }
    else if (command.equalsNoCase("console"))
    {
      //a_Start a thread with main service proc and read keyboard input to exit
      AThread thread(service.MainServiceThreadProc, true);
      std::cout << "Type 'exit' and Enter, to signal service thread to stop." << std::endl;
      AFile_IOStream ios;
      AString input;
      while (!input.equalsNoCase("exit"))
      {
        ios.readLine(input);
      }
      thread.setRun(false);  //a_Flag thread to stop execution
      std::cout << "Thread signaled to stop." << std::flush;
      
      //a_Wait until it exits gracefully
      while(thread.isRunning())
      {
        AThread::sleep(100);
        std::cout << "." << std::flush;
      }
      std::cout << "Bye." << std::flush;
    }
    else
      std::cerr << "Unknown command: " << command <<std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception" << std::endl;
  }
  service.wait();

  return 0;
}
