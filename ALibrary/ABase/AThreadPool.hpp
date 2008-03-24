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
  //! List of thread pointers
  typedef std::list<AThread *> THREADS;

public:
  /*!
  Create pool based on a threadproc (which becomes threadproc of the template thread)

  @param threadproc for each newly created thread
  @param threads to maintain active simultaneously
  @param pThis pointer to the thread that is retrieved with thread.getThis()
  @param pParameter pointer to the thread that is retrieved with thread.getParameter()
  */
  AThreadPool(
    AThread::ATHREAD_PROC *threadproc, 
    int threads = 1, 
    ABase *pThis = NULL, 
    ABase *pParameter = NULL
  );
  
  /*!
  dtor will signal threads to stop and after a short wait terminate them
  It may be cleaner to issue a stop() then wait until getRunningThreadCount() == 0
  */
  virtual ~AThreadPool();

  /*!
  Set total number of threads to create after start() is called
  After this # of threads is created over time the pool stops creating new threads
  This is an iteration count that is independent of the thread count that run simultaneously
  Once this count is zero equivalent to setCreatingNewThreads(false) is called
  Initially set to AConstant::npos and equivalent to infinity
  
  @param count
  */
  void setTotalThreadCreationCount(size_t count);

  /*!
  Get total threads left to create
  */
  size_t getTotalThreadCreationCount() const;

  /*!
  Start thread pool
  */
  void start();
  
  /*
  Stop new threads from being created if they exit
  Allow existing threads to run their course
  Use getRunningThreadCount() to see how many are still running
  This is a graceful stop and terminate will never be called on any thread
  Once all threads have been created and executed, the main threadpool manager stops

  @param b true - Set thread monitor to create new threads if some threads exit
           false - Do not create new threads after they exit
  */
  void setCreatingNewThreads(bool b = false);

  /*!
  Stop thread pool and terminate threads that refuse to stop
  Should be used if graceful stop if not viable 
  */
  void stop();

  /*!
  How many threads to keep active
  Will create new threads as need
  If new count is less than existing threads will be signaled to stop and if they don't stop in time they will be terminated
  */
  void setThreadCount(size_t);
  
  /*!
  Current active physical thread count
  */
  size_t getThreadCount() const;
  
  /*!
  Number of th existing threads that are flagged as still running
  */
  size_t getRunningThreadCount();

  /*!
  How long the monitor sleeps between monitoring cycles
  */
  u4 getMonitorCycleSleep() const;    
  void setMonitorCycleSleep(u4 sleeptime);

  /*!
  AThread this pointer and parameter for getThis() and getParameter()
  All newly created threads get these values, does not affect already running threads
  */
  void setThis(ABase *);
  void setParameter(ABase *);

  /*!
  AThread this pointer and parameter for getThis() and getParameter()
  All newly created threads get these values
  */
  ABase *getThis();
  ABase *getParameter();

  /*!
  Access the thtread container
  This is not very safe unless the SyncObject is locked during use

  {
    ALock lock(threadPool.useSync());
    const AThreadPool::THREADS& threads = threadPool.getThreads();
    // do stuff here with threads
    // during this time the thread monitor thread will not make any changes
    // this may affect thread pool execution due to synchronization
  }
  */
  const AThreadPool::THREADS& getThreads() const;

  /*!
  ASynchronization object that blocks the monitor thread
  MUST lock when using getTheads() call to maintain some consistency
  @see AThreadPool::getThreads()
  */
  ASynchronization& useSync();

  /*!
  Thread pool timer, starts when manager starts and stops when it stops/exits
  */
  const ATimer& getThreadPoolTimer() const;

  /*!
  AEmittable
  */
  virtual void emit(AOutputBuffer&) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  //Thread container
  THREADS m_Threads;

  //Threads to run
  size_t m_threadCount;

private:
  AThreadPool() {}

  // threadproc to monitor the thread pool
  static u4 _threadprocMonitor(AThread& thread);
  AThread m_MonitorThread;
  u4 m_monitorCycleSleep;

  // Flag if new threads are to be created
  bool m_CreateNewThreads;
  
  // Total number of threads to create
  size_t m_TotalThreadCreationCount;

  // Parameters needed for creating new threads
  AThread::ATHREAD_PROC *mp_threadproc;
  ABase *mp_This;
  ABase *mp_Parameter;

  // Synchronization for any pool related changes
  ASync_CriticalSection m_SynchObjectThreadPool;

  // Timer for the runtime of the pool
  ATimer m_ThreadPoolTimer;
};

#endif //INCLUDED__AThreadPool_HPP__
