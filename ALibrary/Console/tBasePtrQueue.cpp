#include "ABasePtrQueue.hpp"
#include "AString.hpp"
#include "AThreadPool.hpp"
#include "ASync_CriticalSection.hpp"
#include "ARandomNumberGenerator.hpp"

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

int main()
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

  return 0;
}
