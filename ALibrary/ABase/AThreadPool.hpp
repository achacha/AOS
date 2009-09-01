/*
Written by Alex Chachanashvili

$Id$
*/
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

  //! Threadpool events
  enum Event
  {
    MonitorStarting = 1,         //! Monitor thread is starting
    MonitorSleeping,             //! Monitor thread is sleeping until next cycle
    MonitorCreatedThread,        //! Monitor created an additional thread
    MonitorFlagToStopThread,     //! Monitor is flagging a thread to stop
    MonitorCheckStopThread,      //! Checking if a thread flagged for stop is finished
    MonitorDestroyedThread,      //! Monitor destroyed an existing thread
    MonitorFinishedIterations,   //! Monitored threads finished set number of iterations
    MonitorExiting               //! Monitor thead is exiting
  };

  //! Callback to an event
  typedef void (*THREADPOOL_CALLBACK)(AThreadPool&, AThreadPool::Event e);

public:
  /*!
  Create pool based on a threadproc (which becomes threadproc of the template thread)

  @param threadproc for each newly created thread
  @param threads to maintain active simultaneously
  @param pThis pointer to the thread that is retrieved with thread.getThis()
  @param pParameter pointer to the thread that is retrieved with thread.getParameter()
  @param pCallback to call or NULL to disable callbacks
  */
  AThreadPool(
    AThread::ATHREAD_PROC *threadproc, 
    int threads = 1, 
    ABase *pThreadThis = NULL, 
    ABase *pThreadParameter = NULL,
    THREADPOOL_CALLBACK pCallback = NULL
  );
  
  /*!
  dtor will signal threads to stop and after a short wait terminate them
  It may be cleaner to issue a stop() then wait until getRunningThreadCount() == 0
  */
  virtual ~AThreadPool();

  /*!
  Sets a callback handler

  @param pCallback or NULL to clear
  */
  void setThreadPoolManagerCallback(THREADPOOL_CALLBACK pCallback = NULL);
  
  /*!
  Waits until monitor thread exits

  @param sleepTime applicable to windows to sleep between polling if the thread is still alive
  */
  void waitForThreadsToExit(u4 sleepTime = 200);

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

  @return total threads created
  */
  size_t getTotalThreadCreationCount() const;

  /*!
  Start thread pool
  */
  virtual void start();
  
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
  Sets the run flag on all running threads

  @param isRun propagated to all running threads
  */
  void setRunStateOnThreads(bool isRun);
  
  /*!
  Stop thread pool and terminate threads that refuse to stop
  Should be used if graceful stop if not viable 
  */
  virtual void stop();

  /*!
  How many threads to keep active
  Will create new threads as need
  If new count is less than existing threads will be signaled to stop and if they don't stop in time they will be terminated
  
  @param count of threads to keep active
  */
  void setThreadCount(size_t count);
  
  /*!
  Current desired thread count
  This is the thread count to maintain not how many are active or running
  
  @return desired active thread count
  */
  size_t getThreadCount() const;
  
  /*
  Number of active threads (may or may not be running)

  @return current actual active thread count
  */
  size_t getActiveThreadCount();

  /*!
  Number of the active threads that are flagged as also still running
  
  @return current actual active thread count also flagged as running
  */
  size_t getRunningThreadCount();

  /*!
  Set how long the monitor sleeps between monitoring cycles

  @return sleep time in milliseconds
  */
  u4 getMonitorCycleSleep() const;    

  /*!
  Set how long the monitor sleeps between monitoring cycles

  @param sleeptime in milliseconds
  */
  void setMonitorCycleSleep(u4 sleeptime);

  /*!
  AThread this pointer accessed with getThis()
  All newly created threads get the value, does not affect already running threads
  */
  void setThis(ABase *);

  /*!
  AThread parameter accessed with getParameter()
  All newly created threads get the value, does not affect already running threads
  */
  void setParameter(ABase *);

  /*!
  Access user set this pointer
  Can be anything and can be used when thread is part of another object

  e.g.
  u4 threadproc(AThread& thread)
  {
    MyObject *pObject = dynamic_cast<ABase *>(thread.getThis());
  }

  @return user set this pointer
  */
  ABase *getThis();

  /*!
  Access user set parameter pointer
  Can be anything

  e.g.
  u4 threadproc(AThread& thread)
  {
    MyObject *pObject = dynamic_cast<ABase *>(thread.getParameter());
  }

  @return user set pointer
  */
  ABase *getParameter();

  /*!
  Set a callback handler
  */

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
  
  @return container of AThread objects
  */
  const AThreadPool::THREADS& getThreads() const;

  /*!
  ASynchronization object that blocks the monitor thread
  MUST lock when using getTheads() call to maintain some consistency
  
  @see AThreadPool::getThreads()
  @return synchronization object for this pool
  */
  ASynchronization& useSync();

  /*!
  Thread pool timer, starts when manager starts and stops when it stops/exits

  @return constant ATimer object reference
  */
  const ATimer& getThreadPoolTimer() const;

  /*!
  AEmittable

  @param target to emit into
  */
  virtual void emit(AOutputBuffer& target) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  Create pool based on a threadproc (which becomes threadproc of the template thread)
  Allows derived class to override monitor threadproc

  @param threadproc for each newly created thread
  @param threads to maintain active simultaneously
  @param pThis pointer to the thread that is retrieved with thread.getThis()
  @param pParameter pointer to the thread that is retrieved with thread.getParameter()
  @param threadprocMonitor (leave as default for most cases) custom threadpool monitor thread
         (if custom monitoring is required then copy existing _threadprocDefaultMonitor function and enhance it)
  */
  AThreadPool(
    AThread::ATHREAD_PROC *threadproc, 
    int threads, 
    ABase *pThis, 
    ABase *pParameter,
    AThread::ATHREAD_PROC *threadprocMonitor,
    THREADPOOL_CALLBACK pCallback = NULL
  );

  //Thread container
  THREADS m_Threads;

  //Threads to run
  size_t m_DesiredThreadCount;

  // threadproc to monitor the thread pool
  static u4 _threadprocDefaultMonitor(AThread& thread);
  AThread::ATHREAD_PROC *mp_threadprocMonitor;
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

  // Callback handler
  void _callCallback(AThreadPool::Event e);
  THREADPOOL_CALLBACK mp_Callback;
  ASync_CriticalSection m_CallbackSynchObject;

private:
  AThreadPool() {}
};

#endif //INCLUDED__AThreadPool_HPP__
