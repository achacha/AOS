
#include "AThread.hpp"

#define ATHREAD_STOP  0x00000001
#define ATHREAD_PAUSE 0x00000002

u4 threadProc(AThread& thread)
{
  AThread::sleep(10000);
  std::cout << "Thread exited." << std::endl;
  return 1;
}


int main(int argc, char **argv)
{
  bool bRun = true;
  
  u4 iteration = 0;
  std::cout << "Creating thread." << std::endl;
  AThread thread(threadProc);
  if (!thread.isThreadActive())
  {
    thread.start();  
  }
  else
  {
    std::cout << "Thread already running." << std::endl;
  }
  std::cout << "Joining thread." << std::endl;
  u4 ret = thread.join();

  std::cout << "Thread returned: " << ret << std::endl;

  return 0;
}
