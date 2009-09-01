
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

ASync_CriticalSection g_ConsoleOutput;

bool g_MonitorExited = false;

void callbackThreadPoolMonitor(AThreadPool& pool, AThreadPool::Event e)
{
  switch(e)
  {
    case AThreadPool::MonitorStarting:
    {
      ALock lock(g_ConsoleOutput);
      std::cout << "[Creating monitor]" << std::flush;
    }
    break;

    case AThreadPool::MonitorCreatedThread:
    {
      ALock lock(g_ConsoleOutput);
      std::cout << "+[" << pool.getActiveThreadCount() << "]" << std::flush;
    }
    break;

    case AThreadPool::MonitorCheckStopThread:
    {
      ALock lock(g_ConsoleOutput);
      std::cout << "?[" << pool.getActiveThreadCount() << "]" << std::flush;
    }
    break;

    case AThreadPool::MonitorFlagToStopThread:
    {
      ALock lock(g_ConsoleOutput);
      std::cout << ">[" << pool.getActiveThreadCount() << "]" << std::flush;
    }
    break;

    case AThreadPool::MonitorDestroyedThread:
    {
      ALock lock(g_ConsoleOutput);
      std::cout << "-[" << pool.getActiveThreadCount() << "]" << std::flush;
    }
    break;

    case AThreadPool::MonitorFinishedIterations:
    {
      ALock lock(g_ConsoleOutput);
      std::cout << "[Iterations done]" << std::flush;
    }
    break;

    case AThreadPool::MonitorSleeping:
    {
      ALock lock(g_ConsoleOutput);
      std::cout << "[Monitor is sleeping]" << std::flush;
    }
    break;

    case AThreadPool::MonitorExiting:
    {
      ALock lock(g_ConsoleOutput);
      std::cout << "[Exiting monitor, type 'exit' and ENTER]" << std::endl;
      g_MonitorExited = true;
    }
    break;

    default:
    {
      ALock lock(g_ConsoleOutput);
      std::cout << "Wut? e=" << e << std::endl;
    }
  }
}

u4 mythreadproc(AThread& thread)
{
  CommonData *pData = static_cast<CommonData *>(thread.getParameter());
  AASSERT(NULL, pData);

  u4 id = thread.getId();
  thread.setRunning(true);

  {
    {
      ALock lock(pData->m_cs);
      pData->m_count++;
    }

    {
      ALock lock(g_ConsoleOutput);
      std::cout << "[" << AString::fromU4(thread.getId()) << "," << pData->m_count << "]" << std::flush;
    }

    AThread::sleep(300);
  }

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