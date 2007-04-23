#ifndef INCLUDED__AThreadPool_HPP__
#define INCLUDED__AThreadPool_HPP__

#include "ADebugDumpable.hpp"
#include "AThread.hpp"
#include "ASync_CriticalSection.hpp"

/*!
Ability to manage a homogenous pool of threads under one facade
*/
class ABASE_API AThreadPool : public ADebugDumpable
{
public:
  /*!
  Create pool based on a threadproc (which becomes threadproc of the template thread)
  */
  AThreadPool(AThread::ATHREAD_PROC threadproc, int threadCount = 1, void *pThis = NULL, void *pParameter = NULL);
  
  /*!
  dtor will signal threads to stop and after a short wait terminate them
  It may be cleaner to issue a stop() then wait until getRunningThreadCount() == 0
  */
  virtual ~AThreadPool();

  /*!
  Pool control
  */
  void start();
  void stop();
  void setThreadCount(size_t);             //a_How many threads to keep active
  size_t getThreadCount() const;           //a_Threads at this moments
  size_t getRunningThreadCount();          //a_How many have flagged themselves as running
  
  /*!
  How long the monitor sleeps between monitoring cycles
  */
  u4 getMonitorCycleSleep() const;    
  void setMonitorCycleSleep(u4 sleeptime);

  /*!
  AThread this pointer and parameter for getThis() and getParameter()
  All newly created threads get these values, does not affect already running threads
  */
  void setThis(void *);
  void setParameter(void *);

protected:
  //Thread container
  typedef std::list<AThread *> THREADS;
  THREADS m_Threads;

  //Threads to run
  size_t m_threadCount;

private:
  AThreadPool() {}

  //threadproc to monitor the thread pool
  static u4 _threadprocMonitor(AThread& thread);
  AThread m_MonitorThread;
  u4 m_monitorCycleSleep;

  /*
  Parameters needed for creating new threads
  */
  AThread::ATHREAD_PROC *mp_threadproc;
  void *mp_This;
  void *mp_Parameter;

  ASync_CriticalSection m_SynchObjectThreadPool;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AThreadPool_HPP__
