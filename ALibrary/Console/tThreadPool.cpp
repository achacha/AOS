
#include "AString.hpp"
#include "AThreadPool.hpp"
#include "ALock.hpp"
#include "AFile_IOStream.hpp"
#include "ASync_CriticalSection.hpp"

class CommonData: public ABase
{
public:
  CommonData() : m_count(0) {}
  int m_count;
  ASync_CriticalSection m_cs;
};

bool g_MonitorExited = false;

void callbackThreadPoolMonitor(AThreadPool& pool, AThreadPool::Event e)
{
  switch(e)
  {
    case AThreadPool::MonitorStarting:
      std::cout << "Creating monitor" << std::endl;
    break;

    case AThreadPool::MonitorCreatedThread:
      std::cout << "++thread" << std::endl;
    break;

    case AThreadPool::MonitorDestroyedThread:
      std::cout << "--thread" << std::endl;
    break;

    case AThreadPool::MonitorFinishedIterations:
      std::cout << "Iterations done" << std::endl;
    break;

    case AThreadPool::MonitorExiting:
      std::cout << "Exiting monitor" << std::endl;
      g_MonitorExited = true;
    break;

    default:
      std::cout << "Wut? e=" << e << std::endl;
  }
}

u4 mythreadproc(AThread& thread)
{
  CommonData *pData = static_cast<CommonData *>(thread.getParameter());
  AASSERT(NULL, pData);

  u4 id = thread.getId();
  thread.setRunning(true);
//  do
  {
    {
      ALock lock(pData->m_cs);
      pData->m_count++;
      std::cout << "[" << AString::fromU4(thread.getId()) << "," << pData->m_count << "]" << std::flush;
    }
    AThread::sleep(300);
  }
//  while (thread.isRun());
  thread.setRunning(false);

  return 0;
}

int main()
{
  CommonData data;
  AThreadPool threadpool(mythreadproc, 20, NULL, &data, callbackThreadPoolMonitor);
  threadpool.setTotalThreadCreationCount(30);
  threadpool.start();

  AFile_IOStream io;
  AString str;
  while (!g_MonitorExited && AConstant::npos != io.readLine(str))
  {
    if (str.equalsNoCase("exit"))
      break;
    AThread::sleep(1000);
  }

  threadpool.stop();

  return 0;
}