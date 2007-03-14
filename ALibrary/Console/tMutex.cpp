#include <AThread.hpp>
#include <ALock.hpp>
#include <AMutex.hpp>
#include <ACriticalSection.hpp>
#include <AFile_IOStream.hpp>

AMutex gcs_ThreadSynch("my_mutex_000");
//ACriticalSection gcs_ThreadSynch;

u4 __threadWriter(AThread& thread)
{
  thread.setRunning(true);
  while(thread.isRun())
  {
    {
      ALock lock(gcs_ThreadSynch);
      std::cerr << "[Locked" << std::flush;
      AThread::sleep(3000);
      std::cerr << "]" << std::endl;
    }
    AThread::sleep(2000);
    if (!thread.isRun()) break;
    AThread::sleep(2000);
    if (!thread.isRun()) break;
    AThread::sleep(2000);
  }
  thread.setRunning(false);
  
  return 0x0;
}

u4 __threadReader(AThread& thread)
{
  thread.setRunning(true);
  while(thread.isRun())
  {
    ALock lock(gcs_ThreadSynch);
    std::cerr << "R" << std::flush;
    AThread::sleep(500);
  }
  thread.setRunning(false);
  
  return 0x0;
}

int main()
{  
  //a_Create thread that locks mutex every N seconds for M seconds
  AThread threadReader(__threadReader, true);
  AThread threadWriter(__threadWriter, true);

  //a_Loop and try to lock mutex every K seconds << N or M
  AFile_IOStream iosfile;
  AString str;
  iosfile.write("\r\n>", 3);
  while (iosfile.readLine(str) != AString::npos)
  {
    if (str == "exit")
    {
      threadReader.setRun(false);
      threadWriter.setRun(false);
      while(threadReader.isRunning() || threadWriter.isRunning())
      {
        AThread::sleep(200);
        std::cout << "!" << std::flush;
      }
      break;
    }

    iosfile.write("\r\n>", 3);
  }

  return 0x0;
}
