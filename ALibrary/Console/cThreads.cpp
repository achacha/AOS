
#include "AThread.hpp"
#include "AFile_IOStream.hpp"

#define ATHREAD_STOP  0x00000001
#define ATHREAD_PAUSE 0x00000002

u4 threadProc(AThread& thread)
{
  std::cout << "Thread started." << std::endl;
  while (!thread.hasFlag(ATHREAD_STOP))
  {
    if (thread.hasFlag(ATHREAD_PAUSE))
    {
      //a_Thread paused, wake up to see flag settings
      AThread::sleep(500);
    }
    else
    {
      u4 *piter = (u4 *)thread.getParameter();
      (*piter)++;
      AThread::sleep(1000);
    }
  }
  std::cout << "Thread exited." << std::endl;
  return 1;
}


int main(int argc, char **argv)
{
  bool bRun = true;
  
  u4 iteration = 0;
  AThread thread(threadProc);
  thread.setParameter((void *)&iteration);

  AString cmd;
  AFile_IOStream io;
  while (bRun)
  {
    std::cout << ">";
    io.readLine(cmd);
    if (!cmd.compareNoCase("status"))
    {
      if (thread.isThreadActive())
      {
        u4 id = thread.getId();
        u4 iter = *(u4 *)thread.getParameter();
        std::cout << "Thread active, id=" << id << " iteration=" << iter << std::endl;
      }
      else if (thread.wasStarted())
      {
        u4 exitCode = thread.getExitCode();
        u4 iter = *(u4 *)thread.getParameter();
        std::cout << "Thread exited, return=" << exitCode << " iteration=" << iter << std::endl;
      }
      else
      {
        std::cout << "Thread has never started." << std::endl;
      }
    }
    else if (!cmd.compareNoCase("start"))
    {
      if (!thread.isThreadActive())
      {
        thread.clearFlag(ATHREAD_STOP);
        thread.start();  
      }
      else
      {
        std::cout << "Thread already running." << std::endl;
      }
    }
    else if (!cmd.compareNoCase("pause"))
    {
      thread.setFlag(ATHREAD_PAUSE);
    }
    else if (!cmd.compareNoCase("resume"))
    {
      thread.clearFlag(ATHREAD_PAUSE);
    }
    else if (!cmd.compareNoCase("stop"))
    {  
      if (thread.isThreadActive())
      {
        thread.setFlag(ATHREAD_STOP);
        std::cout << "Trying to stop" << std::flush;
        while (thread.isThreadActive())
        {
          AThread::sleep(100);
          std::cout << "." << std::flush;
        }
        std::cout << std::endl;
      }
    }
    else if (!cmd.compareNoCase("exit"))
    {
      if (thread.isThreadActive())
      {
        thread.setFlag(ATHREAD_STOP);
        std::cout << "Trying to stop" << std::flush;
        while (thread.isThreadActive())
        {
          AThread::sleep(100);
          std::cout << "." << std::flush;
        }
        std::cout << std::endl;
      }

      bRun = false;
    }

    cmd.clear();
  }

  return 0;
}
