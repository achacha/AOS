#include <AThread.hpp>
#include <AThreadPool.hpp>
#include <ALock.hpp>
#include <ASync_ReadWriteLock.hpp>
#include <AFile_IOStream.hpp>
#include <ARandomNumberGenerator.hpp>

ASync_ReadWriteLock g_ReadWriteLock;

u4 __threadWriter(AThread& thread)
{
  thread.setRunning(true);
  while(thread.isRun())
  {
    {
      std::cout << "{_W" << AString::fromU4(thread.getId()) << std::flush;
      ALock lock(g_ReadWriteLock.getWriteLock());
      std::cout << "writing..." << std::flush;
      AThread::sleep(3000);
      std::cout << "W_}" << std::endl;
    }
    AThread::sleep(ARandomNumberGenerator::get().nextRange(4000,2000));
  }
  thread.setRunning(false);
  
  return 0x0;
}

u4 __threadReader(AThread& thread)
{
  thread.setRunning(true);
  while(thread.isRun())
  {
    {
      std::cout << "[_R" << AString::fromU4(thread.getId()) << "_" << std::flush;
      ALock lock(g_ReadWriteLock);
      std::cout << "reading_]" << std::flush;
    }
    AThread::sleep(ARandomNumberGenerator::get().nextRange(800,200));
  }
  thread.setRunning(false);
  
  return 0x0;
}

int main()
{  
  //a_Create thread that locks mutex every N seconds for M seconds
  AThreadPool readerPool(__threadReader, 3);
  AThreadPool writerPool(__threadWriter, 2);
  readerPool.start();
  writerPool.start();

  //a_Loop and try to lock for write every K seconds << N or M
  AFile_IOStream iosfile;
  AString str;
  while (iosfile.readLine(str) != AConstant::npos)
  {
    if (str == "exit")
    {
      int tries = 10;
      readerPool.stop();
      writerPool.stop();
      while(readerPool.getRunningThreadCount() > 0 || writerPool.getRunningThreadCount() > 0 )
      {
        AThread::sleep(300);
        std::cout << "!" << std::flush;
      }
      break;
    }
    
    str.clear();
  }

  return 0x0;
}
