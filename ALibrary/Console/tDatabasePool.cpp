#include "ADatabasePool.hpp"
#include "ALog_AFile.hpp"
#include "AThread.hpp"
#include "ARandomNumberGenerator.hpp"
#include "AFile_IOStream.hpp"
#include "AException.hpp"
#include "ACriticalSection.hpp"
#include "AResultSet.hpp"
#include "AMySQLServer.hpp"

struct PARAMS
{
  ALog *pLog;
  ADatabasePool *pDBPool;
};

u4 threadprocDBQuery(AThread& thread)
{
  thread.setRunning(true);
  try
  {
    PARAMS *pParams = (PARAMS *)thread.getParameter();
    ALog *plog = pParams->pLog;
    ADatabasePool *pDBPool = pParams->pDBPool;

    AString pre = AString("Thread")+AString::fromInt(thread.getId());

    ALog::ScopeEvents& se = plog->addScopeEvents(pre);

    int i = 0;
    AString query("SELECT * from aos.global;"), error;
    AResultSet rs;
    se.add(AString("Start"), ALog::INFO);
    AString strRS;
    while(thread.isRun())
    {
      if (pDBPool->executeSQL(query, rs, error) == AConstant::npos)
      {
        plog->add(pre, error);
      }
      else
      {
        rs.emit(strRS);
        plog->add(pre, AString::fromInt(i), strRS);
        strRS.clear();
      }

      ++i;
      AThread::sleep(ARandomNumberGenerator::get().nextRange(2000, 400));
      rs.clear();
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

//#define SINGLE_THREADED
#define VERY_MULTI_THREADED
int main()
{
  try
  {
    AUrl url("mysql://aos:aos@localhost/aos/");
    ADatabase *pDB = new AMySQLServer(url);
    ADatabasePool dbpool(pDB, 2);
//    dbpool.debugDump();
    
    ALog_AFile alog(new ACriticalSection(), AFilename("mydblog.txt"));
    alog.setEventMask(ALog::ALL);
    alog.add(AString("Starting threaded log test"), ALog::MESSAGE);

    AThread thread1(threadprocDBQuery);
#ifndef SINGLE_THREADED
    AThread thread2(threadprocDBQuery);
    AThread thread3(threadprocDBQuery);
    AThread thread4(threadprocDBQuery);
#endif
#ifdef VERY_MULTI_THREADED
    AThread thread5(threadprocDBQuery);
    AThread thread6(threadprocDBQuery);
    AThread thread7(threadprocDBQuery);
    AThread thread8(threadprocDBQuery);
    AThread thread9(threadprocDBQuery);
#endif

    PARAMS params;
    params.pLog = &alog;
    params.pDBPool = &dbpool;
    
    thread1.setParameter((void *)&params);  
#ifndef SINGLE_THREADED
    thread2.setParameter((void *)&params);  
    thread3.setParameter((void *)&params);  
    thread4.setParameter((void *)&params);  
#endif
#ifdef VERY_MULTI_THREADED
    thread5.setParameter((void *)&params);  
    thread6.setParameter((void *)&params);  
    thread7.setParameter((void *)&params);  
    thread8.setParameter((void *)&params);  
    thread9.setParameter((void *)&params);  
#endif

    thread1.start();
#ifndef SINGLE_THREADED
    thread2.start();
    thread3.start();
    thread4.start();
#endif
#ifdef VERY_MULTI_THREADED
    thread5.start();
    thread6.start();
    thread7.start();
    thread8.start();
    thread9.start();
#endif

    AFile_IOStream io;
    AString input;
    bool notDone = true;
    while (notDone)
    {
      std::cout << "'exit' to exit >" << std::flush;
      io.readLine(input);

      if (input.equals("exit"))
      {
        notDone = false;
      }
      else if (input.equals("db"))
      {
        dbpool.debugDump();
      }
      input.clear();
    }

    thread1.setRun(false);
#ifndef SINGLE_THREADED
    thread2.setRun(false);
    thread3.setRun(false);
    thread4.setRun(false);
#endif
#ifdef VERY_MULTI_THREADED
    thread5.setRun(false);
    thread6.setRun(false);
    thread7.setRun(false);
    thread8.setRun(false);
    thread9.setRun(false);
#endif

    while(
      thread1.isRunning()
#ifndef SINGLE_THREADED
      || thread2.isRunning()
      || thread3.isRunning()
      || thread4.isRunning()
#endif
#ifdef VERY_MULTI_THREADED
      || thread5.isRunning()
      || thread6.isRunning()
      || thread7.isRunning()
      || thread8.isRunning()
      || thread9.isRunning()
#endif
      )
    {
      AThread::sleep(100);
      std::cout << "." << std::flush;
    }

    dbpool.debugDump();
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

  return 0;
}
