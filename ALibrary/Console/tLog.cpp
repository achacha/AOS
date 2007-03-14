#include "ALog_AFile.hpp"
#include "AThread.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AFile_Physical.hpp"
#include "AFile_IOStream.hpp"
#include "AFile_AString.hpp"
#include "AException.hpp"
#include "ACriticalSection.hpp"
#include "AThreadPool.hpp"

u4 threadprocLogWriter(AThread& thread)
{
  thread.setRunning(true);
  try
  {
    ALog *plog = (ALog *)thread.getParameter();
    AString pre = AString("Thread")+AString::fromInt(thread.getId());

    ALog::ScopeEvents& se = plog->addScopeEvents(pre);

    int i = 0;
    se.add(AString("Start"), ALog::INFO);
    while(thread.isRun())
    {
      plog->add(pre, AString::fromInt(i));
      ++i;
      AThread::sleep(ARandomNumberGenerator::get().nextRange(1000, 400));
    }
    se.add(AString("Stop"), ALog::INFO);
  }
  catch(AException& ex)
  {
    std::cerr << ex.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Unknown exception." << std::endl;
  }
  thread.setRunning(false);
  return 0;
}

void loopTest(ALog& alog)
{
  for (int i=0; i<3; ++i)
  {
    AString out = AString("LogEntry 1:")+AString::fromInt(i);
    alog.append(out);
  }
}

#define USE_AFile_Physical
//#define USE_AFile_AString
int main()
{
  AAutoPtr<AFile_AString> pFile(new AFile_AString());
  try
  {
#if defined(USE_AFile_Physical)
    ALog_AFile alog(new ACriticalSection(), AFilename("./test/mylog.txt"));
#elif defined(USE_AFile_AString)
    ALog_AFile alog(new ACriticalSection(), pFile.get());
#else
#pragma error("One define must exist")
#endif
    alog.setLoggerMaxFileSize(1024);
    alog.setEventMask(ALog::ALL);
    alog.setLoggerCycleSleep(300);
    alog.add(AString("Starting threaded log test"), ALog::MESSAGE);
    AThreadPool tpool(threadprocLogWriter, 6);
    tpool.setParameter(&alog);
    tpool.setMonitorCycleSleep(500);
    tpool.start();

    AFile_IOStream io;
    AString str;
    std::cout << "Logging... 'exit' to exit >" << std::flush;
    while (AConstant::npos != io.readLine(str))
    {
      if (str.equals("exit"))
        break;
    }
    tpool.stop();
  }
  catch(AException& ex)
  {
    std::cout << ex.what() << std::endl;
  }
  catch(const char *pccex)
  {
    std::cout << pccex << std::endl;
  }
  catch(...)
  {
    std::cout << "Unknown exception caught." << std::endl;
  }

#if defined(USE_AFile_Physical)
#elif defined(USE_AFile_AString)
  std::cout << pFile->useAString() << std::endl;
#endif

  return 0;
}
