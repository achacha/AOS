#include "AThread.hpp"
#include "ATimer.hpp"
#include "AFile_IOStream.hpp"
#include "AException.hpp"

u4 threadprocSample(AThread& thread)
{
  thread.setRunning(true);  // Flag thread as starting to run
  thread.useExecutionState().assign("Starting to execute");  // Message that stores what a thread is doing (optional)
  try
  {
    do
    {
      thread.useExecutionState().assign("Doing some work");
      
      // Do some work
      std::cout << "." << std::flush;

      AThread::sleep(500);  // Sleep in a loop to manage CPU utilitzation (optional)
    }
    while (thread.isRun());   // Execute in a loop while needed to run
    thread.useExecutionState().assign("Exited");
  }
  catch(AException& ex)
  {
    thread.useExecutionState().assign(ex.what());
  }
  catch(...)
  {
    thread.useExecutionState().assign("Unknown exception caught");
  }
  thread.setRunning(false); // Flag thread as stopping
  return 0;
}

int main()
{
  std::cout << "Thread created, type 'exit' to signal it to stop." << std::endl;
  AThread thread(threadprocSample, true);

  AFile_IOStream io;
  AString str;
  while(AConstant::npos != io.readLine(str))
  {
    if (str.equalsNoCase("exit"))
      break;

    AThread::sleep(500);
  }

  thread.setRun(false);
  int retries = 10;
  while(retries > 0 && thread.isRunning())
  {
    //a_Wait for the thread to gracefully finish and exit
    //a_AThread::terminate should be avoided at all cost, it's brute force (see Microsoft SDK documentation)
    AThread::sleep(100);  //a_Small sleep value to prevent tight empty loops
    --retries;
  }
  if (!retries && thread.isRunning())
    thread.terminate();  //a_The sledgehammer, thread did not exit in allotted time

  std::cout << "Done." << std::endl;

  return 0;
}
