#include "ACache_FileSystem.hpp"
#include "ARope.hpp"
#include "AXmlElement.hpp"
#include "AThreadPool.hpp"
#include "AException.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AFile_IOStream.hpp"

ACache_FileSystem g_Cache;

#define WORK(f) do { g_Cache.get(AFilename(f));\
      AThread::sleep(ARandomNumberGenerator::get().nextRange(800, 100));\
      std::cout.put('.'); } while(0)


u4 _threadproc(AThread& thread)
{
  thread.setRunning(true);
  while(thread.isRun())
  {
    try
    {
      WORK("C:/Code/_debug/aos_root/static/index.html");
      WORK("C:/Code/_debug/aos_root/static/login_md5.html");
      WORK("C:/Code/_debug/aos_root/static/login_md5.html");
      WORK("C:/Code/_debug/aos_root/static/aos.html");
      WORK("C:/Code/_debug/aos_root/static/donotexist.foo");
      WORK("C:/Code/_debug/aos_root/static/login_md5.html");
      WORK("C:/Code/_debug/aos_root/static/aos.html");
      WORK("C:/Code/_debug/aos_root/static/donotexist.foo");
    }
    catch(AException& e)
    {
      std::cerr << e.what() << std::endl;
      break;
    }
    catch(...)
    {
      std::cerr << "Unknown exception." << std::endl;
      break;
    }
  }
  thread.setRunning(false);
  return 0;
}

int main()
{
  AThreadPool worker(_threadproc, 1);
  worker.start();

  AFile_IOStream iosfile;
  AString str;
  while (iosfile.readLine(str) != AConstant::npos)
  {
    if (str == "exit")
    {
      worker.stop();
      while(worker.getRunningThreadCount() > 0)
      {
        std::cout << "?" << std::flush;
      }
      break;
    }
    
    str.clear();
  }


  return 0;
}