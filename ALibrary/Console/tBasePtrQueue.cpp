#include "ABasePtrQueue.hpp"
#include "AString.hpp"
#include "AThreadPool.hpp"
#include "ASync_CriticalSection.hpp"
#include "ARandomNumberGenerator.hpp"
#include "ATimer.hpp"

u4 threadproc(AThread& thread)
{
  ABasePtrQueue *pQueue = dynamic_cast<ABasePtrQueue *>(thread.getParameter());
  
  thread.setRunning(true);
  while (thread.isRun())
  {
    AString *p = dynamic_cast<AString *>(pQueue->pop());
    if (p)
    {
      AThread::sleep(ARandomNumberGenerator::get().nextRange(50));
      ALock lock(pQueue->useSync());
      std::cout << *p << std::endl;
      delete p;
    }
    else
      AThread::sleep(100);
  }
  thread.setRunning(false);
  
  return 0;
}

void threadedTest()
{
  ABasePtrQueue queue(new ASync_CriticalSection());
  queue.push(new AString("item000"));
  queue.push(new AString("item001"));
  queue.push(new AString("item002"));
  queue.push(new AString("item003"));
  queue.push(new AString("item004"));
  queue.push(new AString("item005"));
  queue.push(new AString("item006"));
  queue.push(new AString("item007"));

  AThreadPool tpool(threadproc, 3, NULL, &queue);
  tpool.start();
  while (!queue.isEmpty())
    AThread::sleep(100);

  tpool.stop();
}

void performanceTest()
{
  const int ITERATIONS = 100000;
  {
    ATimer timer(true);
    ABasePtrQueue queue;
    for (int i=0; i<ITERATIONS; ++i)
    {
      queue.push(new AString("000"));
    }
    timer.stop();
    std::cout << "internal=" << timer.getInterval() << "ms" << std::endl;

    timer.start();
    for (int i=0; i<ITERATIONS; ++i)
    {
      ABase *p = queue.pop();
      delete p;
    }
    if (!queue.isEmpty())
      std::cerr << "Consistency failure!" << std::endl;

    timer.stop();
    std::cout << "internal=" << timer.getInterval() << "ms" << std::endl;

  }

  {
    ATimer timer(true);
    ABasePtrQueue queue(new ASync_CriticalSection());
    for (int i=0; i<ITERATIONS; ++i)
    {
      queue.push(new AString("000"));
    }
    timer.stop();
    std::cout << "internal=" << timer.getInterval() << "ms" << std::endl;

    timer.start();
    for (int i=0; i<ITERATIONS; ++i)
    {
      ABase *p = queue.pop();
      delete p;
    }
    if (!queue.isEmpty())
      std::cerr << "Consistency failure!" << std::endl;

    timer.stop();
    std::cout << "internal=" << timer.getInterval() << "ms" << std::endl;

  }

  {
    ATimer timer(true);
    std::deque<ABase *> queue;
    for (int i=0; i<ITERATIONS; ++i)
    {
      queue.push_back(new AString("000"));
    }
    timer.stop();
    std::cout << "std::deque=" << timer.getInterval() << "ms" << std::endl;

    timer.start();
    for (int i=0; i<ITERATIONS; ++i)
    {
      ABase *p = queue.front();
      queue.pop_front();
      delete p;
    }
    if (!queue.empty())
      std::cerr << "std::Consistency failure!" << std::endl;

    timer.stop();
    std::cout << "std::deque=" << timer.getInterval() << "ms" << std::endl;
  }
}

int main()
{
  //threadedTest();
  performanceTest();
  return 0;
}