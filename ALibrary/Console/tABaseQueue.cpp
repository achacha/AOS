#include "AException.hpp"
#include "ABasePtrQueue.hpp"
#include "AThreadPool.hpp"
#include "AThread_NOP.hpp"
#include "ATextGenerator.hpp"
#include "ASync_CriticalSection.hpp"

u4 threadprocWork(AThread& thread)
{
  ABasePtrQueue *pQ = dynamic_cast<ABasePtrQueue *>(thread.getParameter());

  int i;
  for (i=0; i<100; ++i)
  {
    std::cout << "a" << std::flush;
    pQ->pushBack(new AString(AString::fromU4(thread.getId())+AString::fromInt(i)));
  }
  std::cout << std::endl;

  for (i=0; i<5; ++i)
  {
    std::cout << "u" << std::flush;
    delete pQ->popFront(1);
    delete pQ->popBack();
    delete pQ->popFront();
    delete pQ->popBack(1);
    delete pQ->popFront();
    delete pQ->popBack(1);
  }

  for (i=0; i<10; ++i)
  {
    std::cout << "d" << std::flush;
    delete pQ->popFront();
    delete pQ->popBack();
  }
  std::cout << std::endl;

  while (!pQ->isEmpty())
  {
    std::cout << "c" << std::flush;
    delete pQ->popFront();
  }

  thread.setRunning(false);
  return 0;
}

int main()
{
  try
  { 
    // Single-threaded test case
    //ABasePtrQueue queue;
    //AThread_NOP thread(threadprocWork, true, NULL, &queue);
    //thread.waitForThreadToExit();

    // Multi-threaded test case
    const int TOTAL_THREADS = 20;
    ABasePtrQueue queue(new ASync_CriticalSection());
    AThreadPool threadPool(threadprocWork, TOTAL_THREADS, NULL, &queue);
    threadPool.setTotalThreadCreationCount(TOTAL_THREADS);
    threadPool.start();
    threadPool.waitForThreadsToExit();

    if (0 != queue.size() && NULL != queue.getHead() && NULL != queue.getTail())
      std::cout << "\r\n!!!Consistency ERROR!!!" << std::endl;
    else
      std::cout << "\r\nAll is well." << std::endl;
  }
  catch(AException& ex)
  {
    std::cerr << ex << std::endl;
  }
  return 0;
}