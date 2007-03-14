#include "ADaemon.hpp"
#include "ASocketListener.hpp"
#include "AFile_Socket.hpp"
#include "AMutex.hpp"
#include "ALock.hpp"

bool g_boolShutdown = false;

//-----------------------------------------------------------------------------------------//
const AString cstrServiceName("ACronDaemon");           //a_USER: The service name
const AString cstrServiceDesc("A cron daemon");           //a_USER: The service description

//a_Control functions (Add content here!)
int controlStartPending()
{
  g_boolShutdown = false;
  return 1;
}

int controlStopPending()
{
  g_boolShutdown = true;
  return 1;
}

int controlInterrogate()
{
  return 1;
}         

int controlShutdown()
{
  return 0;
}

int controlStopped()
{
  return 1;
}

int controlContinuePending()
{
  return 0;
}

int controlRunning()
{
  return 1;
}

int controlPausePending()
{
  return 0;
}

int controlPaused()
{
  return 1;
}

//-----------------------------------------------------------------------------------------//
AMutex g_cleanup("cleanThread");
std::list<AThread *> threads;
ASocketLibrary g_SocketLib;

u4 cleanupThreadProc(AThread& thread)
{
  thread.setRunning();
  while (thread.isRun())
  {
    AThread::sleep(5000);
    {
      ALock lock(g_cleanup);
      std::list<AThread *>::iterator it = threads.begin();
      while (it != threads.end())
      {
        if (!(*it)->isRunning())
        {
          std::list<AThread *>::iterator kill = it;
          ++it;
          AThread *pKill = (*kill);
          threads.erase(kill);
          delete pKill;
        }
      }
    }
  }
  thread.setRunning(false);
  return 0x0;
}

u4 threadProc(AThread& thread)
{
  try
  {
    AFile_Socket& socket = *(AFile_Socket *)thread.getParameter();

    thread.setRunning(true);
    AString input;
    socket.write("> ");
    while (thread.isRun())
    {
      if (AString::npos != socket.readLine(input))
      {
        if (!input.isEmpty())
        {
          if (!input.compare("exit"))
          {
            socket.writeLine("Bye.");
            thread.setRun(false);
          }
          if (!input.compare("shutdown"))
          {
            //a_Shutdown
            socket.writeLine("Shutdown received, attempting to shutdown.\nBye.");
            g_boolShutdown = true;
            thread.setRun(false);
          }
          else
          {
            socket.writeLine(AString("\nECHO: ") + input);
            input.clear();
            socket.write("> ");
          }
        }
      }
      if (thread.isRun())
        AThread::sleep(500);
    }
    socket.close();
  }
  catch(AException&)
  {
  }

  thread.setRunning(false);
  return 0;
}

u4 fcbMainServiceThread(AThread& thread)
{
  thread.setRunning();
  AThread cleanupThread(cleanupThreadProc, true);

  AString error;
  ASocketListener listener;
  while (thread.isRun())
  {
    try
    {
      listener.open(1366);
    }
    catch(const AException& e)
    {
      error = e.what();
    }

    while (thread.isRun())
    {
      if (listener.isAcceptWaiting())
      {
        AFile_Socket *p = new AFile_Socket(listener);

        AThread *t = new AThread(threadProc);
        t->setParameter(p);
        t->start();
        {
          ALock lock(g_cleanup);
          threads.push_back(t);
        }
      }
      else
      {
        AThread::sleep(250);
      }
    }
  }

  cleanupThread.setRun(false);
  int iTimeout = 10;
  while (iTimeout > 0 && cleanupThread.isRunning())
  {
    AThread::sleep(1000);  //a_May have to wait 5 seconds since the sleep time on the cleanup thread is a bit long
  }

  thread.setRunning(false);

  return 0x0;
}

void showUsage()
{
  std::cout << "Usage: this <command>" << std::endl;
  std::cout << "  console - Run service in console mode" << std::endl;
  std::cout << "  invoke  - Invoke (install and/or start) service" << std::endl;
  std::cout << "  install - Installs this service" << std::endl;
  std::cout << "  status  - Returns status of the service" << std::endl;
  std::cout << "  remove  - Stops and removes this service" << std::endl;
  std::cout << "  start   - Starts the service and installs if needed" << std::endl;
  std::cout << "  stop    - Stops the service" << std::endl;

}

void executeServiceThread()
{
  //a_Local invoke
  try
  {
    AThread thread(fcbMainServiceThread, true);

    //a_Wait for it to startup
    while (thread.isRun() && !thread.isRunning())
    {
      AThread::sleep(500);
    }

    //a_Wait for a shutdown or white thread is still running
    while (thread.isRunning()) {
      if (g_boolShutdown)
        thread.setRun(false);
      else
        AThread::sleep(5000);
    }
  }
  catch(AException& e)
  {
    std::cout << e.what() << std::endl;
  }
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    showUsage();
    return -1;
  }
  
  try
  {
    AString command(argv[1]);
    if (!command.compareNoCase("console"))
    {
      controlStartPending();
      executeServiceThread();
      controlStopPending();
    }
    else
    {
      ADaemon daemon(fcbServiceMain, fcbServiceHandler, &cstrServiceName, &cstrServiceDesc);
      daemon.init();
      int result = 0;
      if (!command.compareNoCase("status"))
      {
        std::cout << "My name is:        " << cstrServiceName << std::endl;
        std::cout << "My description is: " << cstrServiceDesc << std::endl;
        std::cout << "Attempting to get status..." << std::endl;
        std::cout << daemon.getStatus() << std::endl;
      }
      else if (!command.compareNoCase("invoke"))
      {
        if (-1 == (result = daemon.invoke()))
          std::cout << "Service invoked. Return: " << result << std::endl;
        else
          std::cout << "Could not invoke service." << std::endl;
      }
      else if (!command.compareNoCase("install"))
      {
        if (result = daemon.installNTService(AString::sstr_Empty))
          std::cout << "Service installed. Return: " << result << std::endl;
        else
          std::cout << "Could not install service." << std::endl;
      }
      else if (!command.compareNoCase("start"))
      {
        if (result = daemon.startNTService())
          std::cout << "Service started. Return: " << result << std::endl;
        else
          std::cout << "Could not start service." << std::endl;
      }
      else  if (!command.compareNoCase("stop"))
      {
        if (result = daemon.stopNTService())
          std::cout << "Service stopped. Return: " << result << std::endl;
        else
          std::cout << "Could not stop service." << std::endl;
      }
      else  if (!command.compareNoCase("remove"))
      {
        if ( result = daemon.removeNTService() )
          std::cout << "Service removed. Return: " << result << std::endl;
        else
          std::cout << "Could not remove service." << std::endl;
      }
      else
      {
        showUsage();
        return -2;
      }
    }
  }
  catch(AException& ex)
  {
    std::cout << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception caught." << std::endl;
  }

  return 0x0;
}
