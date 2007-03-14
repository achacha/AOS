
#include "ASemaphore.hpp"
#include "AMutex.hpp"
#include "ALock.hpp"
#include "AFile_IOStream.hpp"
#include "AThread.hpp"
#include "AException.hpp"

#define THREAD_FLAG_EXIT 0x01
#define THREAD_FLAG_LOCKED 0x02

AMutex g_Output("writeOutput");

u4 lockSemaphoreThreadProc(AThread& thread)
{
  {
    ALock writeLock(g_Output);
    std::cout << "Starting thread and waiting on semaphore..." << std::endl;
  }

  try
  {
    ASemaphore semaphore("Global\\console000", 2);
    ALock lock(semaphore);

    {
      ALock writeLock(g_Output);
      std::cout << "Locking semaphore (" << thread.getParameter() << ")..." << std::endl;
    }

    thread.setFlag(THREAD_FLAG_LOCKED);
    while (!thread.hasFlag(THREAD_FLAG_EXIT))
    {
      AThread::sleep(1000);
    }
  }
  catch(AException& ex)
  {
    std::cerr << ex.what().c_str() << std::endl;
  }

  {
    ALock writeLock(g_Output);
    std::cout << "Semaphore (" << thread.getParameter() << ") is about to be unlocked." << std::endl;
  }

  thread.clearFlag(THREAD_FLAG_LOCKED);
  return 0;
}

int main(int argc, char **argv)
{
  std::vector<AThread *> threads;
  AFile_IOStream io;
  AString input;
  int some_id = 1;
  while (input.compare("exit"))
  {
    {
      ALock writeLock(g_Output);
      std::cout << "'exit', 'list', 'lock', 'unlock <id>' :" << std::flush;
    }
    io.readLine(input);

    if (!input.compare("lock"))
    {
      AThread *p = new AThread(lockSemaphoreThreadProc);
      p->setParameter((void *)some_id);
      p->start();
      threads.push_back(p);
      ++some_id;
    }
    else if (!input.compare("list"))
    {
      {
        ALock writeLock(g_Output);
        for (int i=0; i<threads.size(); ++i)
        {
          std::cout << i << ": " << (threads.at(i)->hasFlag(THREAD_FLAG_LOCKED) ? "locked:" : "waiting:") << threads.at(i)->getParameter() << std::endl;
        }
      }
    }
    else if (!input.find("unlock"))
    {
      int index = input.substr(6).toInt();
      if (index>=0 && index<threads.size())
      {
        AThread *p = threads.at(index);
        p->setFlag(THREAD_FLAG_EXIT);
        std::vector<AThread *>::iterator it = threads.begin();
        it += index;
        threads.erase(it);
      }
    }

  }
  
  return 0;
}
