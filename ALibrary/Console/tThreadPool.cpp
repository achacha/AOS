
#include "AString.hpp"
#include "AThreadPool.hpp"
#include "ALock.hpp"
#include "AFile_IOStream.hpp"
#include "ACriticalSection.hpp"

class CommonData
{
public:
  CommonData() : m_count(0) {}
  int m_count;
  ACriticalSection m_cs;
};

u4 mythreadproc(AThread& thread)
{
  CommonData *pData = static_cast<CommonData *>(thread.getParameter());
  AASSERT(NULL, pData);

  u4 id = thread.getId();
  thread.setRunning(true);
  do
  {
    {
      ALock lock(pData->m_cs);
      pData->m_count++;
      std::cout << "[" << AString::fromU4(thread.getId()) << "," << pData->m_count << "]" << std::flush;
    }
    AThread::sleep(300);
  }
  while (thread.isRun());
  thread.setRunning(false);

  return 0;
}

int main()
{
  CommonData data;
  AThreadPool threadpool(mythreadproc, 20, NULL, &data);
  threadpool.start();

  AFile_IOStream io;
  AString str;
  while (AConstant::npos != io.readLine(str))
  {
    if (str.equalsNoCase("exit"))
      break;
    AThread::sleep(1000);
  }

  threadpool.stop();

  return 0;
}