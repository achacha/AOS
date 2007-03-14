#include "AThread.hpp"
#include "ATimer.hpp"
#include "AException.hpp"

u4 mythreadproc(AThread& thread)
{
  thread.setRunning();         //a_Set thread state as running
  try
  {
    std::cout << "Thread running." << std::endl;
    while (thread.isRun())
    {
      //a_Display a persion ever second
      std::cout << "." << std::flush;
      AThread::sleep(1000);
    }
    std::cout << "Thread detected a request to stop." << std::endl;
  }
  catch(AException& ex)
  {
    // Process AException type (if you throw these)
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    // Process all other exceptions
  }
  std::cout << "Thread setting Running state to false." << std::endl;
  thread.setRunning(false);    //a_Set thread state as not running anymore
  return 0;                    //a_GetExitCode will return this for you once thread exits
}

int main()
{
  std::cout << "Thread created." << std::endl;
  AThread thread(mythreadproc, true);
  ATimer timer(true);              //a_Start timer

  while(timer.getInterval() < 5000)
  {
    AThread::sleep(500);
  }
  
  thread.setRun(false);
  while(thread.isRunning())
  {
    //a_Wait for the thread to gracefully finish and exit
    //a_TerminateThread should be avoided at all cost (see Microsoft SDK documentation)
    AThread::sleep(100);
  }
  std::cout << "Thread exited." << std::endl;

  return 1;
}
