/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AThreadPool.hpp"
#include "ASystemException.hpp"
#include "ALock.hpp"

#define DEFAULT_MONITOR_CYCLE_SLEEP 10
#define DEFAULT_STOP_CYCLE_SLEEP 100
#define DEFAULT_MONITOR_STARTUP_SLEEPTIME 10

void AThreadPool::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_MonitorCycleSleep=" << m_MonitorCycleSleep << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "mp_threadproc=" << AString::fromPointer(mp_threadproc) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_This=" << AString::fromPointer(mp_This) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Parameter=" << AString::fromPointer(mp_Parameter) << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_TotalThreadsCreated=" << m_TotalThreadsCreated << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_TotalThreadCreationCount=" << m_TotalThreadCreationCount  << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_CreateNewThreads=" << AString::fromBool(m_CreateNewThreads) << std::endl;

  {
    ALock lock(const_cast<ASync_CriticalSection *>(&m_SynchObjectThreadPool));
    ADebugDumpable::indent(os, indent+1) << "m_Threads={" << std::endl;
    ADebugDumpable::indent(os, indent+2) << "size()=" << m_Threads.size() << std::endl;
    THREADS::const_iterator cit = m_Threads.begin();
    while (cit != m_Threads.end())
    {
      (*cit)->debugDump(os, indent+2);
      ++cit;
    }
    ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  }

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AThreadPool::AThreadPool(
  AThread::ATHREAD_PROC *threadproc,
  int threadCount,               // = 1
  ABase *pThis,                  // = NULL
  ABase *pParameter              // = NULL
) :
  mp_threadproc(threadproc),
  mp_This(pThis),
  mp_Parameter(pParameter),
  m_MonitorThread(AThreadPool::_threadprocDefaultMonitor, false),
  m_DesiredThreadCount(threadCount),
  m_MonitorCycleSleep(DEFAULT_MONITOR_CYCLE_SLEEP),
  m_CreateNewThreads(true),
  m_TotalThreadCreationCount(AConstant::npos),
  m_TotalThreadsCreated(0)
{
  m_MonitorThread.setThis(this);

  // By default 'this' will be to the thread pool
  if (!mp_This)
    mp_This = this;
}

AThreadPool::AThreadPool(
  AThread::ATHREAD_PROC *threadproc,
  int threadCount,
  ABase *pThis,
  ABase *pParameter,
  AThread::ATHREAD_PROC *threadprocMonitor
) :
  mp_threadproc(threadproc),
  mp_This(pThis),
  mp_Parameter(pParameter),
  m_MonitorThread(threadprocMonitor, false),
  m_DesiredThreadCount(threadCount),
  m_MonitorCycleSleep(DEFAULT_MONITOR_CYCLE_SLEEP),
  m_CreateNewThreads(true),
  m_TotalThreadCreationCount(AConstant::npos)
{
  m_MonitorThread.setThis(this);

  // By default 'this' will be to the thread pool
  if (!mp_This)
    mp_This = this;
}

AThreadPool::~AThreadPool()
{
  try
  {
    if (m_MonitorThread.isRunning())
      stop();
  }
  catch(...) {}
}

void AThreadPool::emit(AOutputBuffer&) const
{
  //TODO: display status, may need a safe way to do this
}

u4 AThreadPool::_threadprocDefaultMonitor(AThread& thread)
{
  AThreadPool *pThis = (AThreadPool *)thread.getThis();
  AASSERT(pThis, pThis);

  bool skipNextSleep = false, shouldSleepMore = false;
  thread.setRunning(true);
  pThis->m_ThreadPoolTimer.start();
  try
  {
    do
    {
      if (
        !skipNextSleep 
        && (pThis->m_Threads.size() == pThis->m_DesiredThreadCount || !pThis->m_TotalThreadCreationCount)
      )
      {
        // Short sleep since desired number of threads are running or we are not creating any more
        AThread::sleep(pThis->m_MonitorCycleSleep);
      }
      else
        skipNextSleep = false;

      // Stopping condition
      if (
             !pThis->m_Threads.size()
          && (
               !pThis->m_TotalThreadCreationCount 
            || !pThis->m_CreateNewThreads
          )
          || !thread.isRun()
      )
      {
        thread.setRun(false);
        break;
      }
      
      {
        ALock lock(pThis->m_SynchObjectThreadPool);
        if (pThis->m_Threads.size() < pThis->m_DesiredThreadCount)
        {
          while (
               pThis->m_CreateNewThreads 
            && pThis->m_TotalThreadCreationCount > 0 
            && pThis->m_Threads.size() < pThis->m_DesiredThreadCount)
          {
            // More
            AASSERT(pThis, pThis->mp_threadproc);
            AASSERT(pThis, ADebugDumpable::isPointerValid(pThis->mp_threadproc));

            AThread *pthread = new AThread(pThis->mp_threadproc, true, pThis->mp_This, pThis->mp_Parameter);
            pThis->m_Threads.push_back(pthread);
            shouldSleepMore = true;                // Allow the thread to start

            
            // Increment total created 
            ++pThis->m_TotalThreadsCreated;

            if (AConstant::npos != pThis->m_TotalThreadCreationCount)
              --pThis->m_TotalThreadCreationCount;
          }
        }
        else if (pThis->m_Threads.size() > pThis->m_DesiredThreadCount)
        {
          // Less
          size_t diff = pThis->m_Threads.size() - pThis->m_DesiredThreadCount;
          THREADS::iterator it = pThis->m_Threads.begin();
          while (diff > 0)
          {
            AASSERT(pThis, it != pThis->m_Threads.end());
            (*it)->setRun(false);
            shouldSleepMore = true;
            ++it;
            --diff;
          }
        }
      }

      // Short sleep to allow time for threads to start up or shut down gracefully
      if (shouldSleepMore)
      {
        AThread::sleep(pThis->m_MonitorCycleSleep);
        shouldSleepMore = false;
      }

      // Terminate any thread that has been flagged for stopping and has not stopped yet
      {
        THREADS::iterator it = pThis->m_Threads.begin();
        while (it != pThis->m_Threads.end())
        {
          AThread *p = *it;
          if (!(*it)->isRunning())
          {
            // Thread somehow died, cleanup
            THREADS::iterator itKill = it;
            ++it;
            pThis->m_Threads.erase(itKill);
            delete p;
          }
          else if (!(*it)->isRun() && (*it)->isRunning())
          {
            THREADS::iterator itKill = it;
            ++it;
            (*itKill)->terminate();
            pThis->m_Threads.erase(itKill);
            delete p;
          }
          else
          {
            ++it;
          }
        }
      }
      if (!pThis->m_TotalThreadCreationCount && !pThis->m_Threads.size())          
      {
        skipNextSleep = true;
      }
    }
    while (thread.isRun());
    pThis->m_ThreadPoolTimer.stop();
  }
  catch(AException& ex)
  {
    thread.useExecutionState().assign(ex.what());
  }
  catch(...)
  {
    thread.useExecutionState().assign("Unknown exception caught");
  }

  thread.setRunning(false);
  return 0;
}

void AThreadPool::start()
{
  if (!m_MonitorThread.isThreadActive() || !m_MonitorThread.isRunning())
  {
    m_CreateNewThreads = true;
    m_TotalThreadsCreated = 0;
    if (!m_TotalThreadCreationCount)
      m_TotalThreadCreationCount = AConstant::npos;
    
    // Start monitor thread and wait until it flags itself as started
    m_MonitorThread.start();
    while (!m_MonitorThread.isRunning())
      AThread::sleep(DEFAULT_MONITOR_STARTUP_SLEEPTIME);
  }
}

void AThreadPool::setCreatingNewThreads(bool b)
{
  m_CreateNewThreads = b;
}

void AThreadPool::setRunStateOnThreads(bool isRun)
{
  // Sets run state on all threads
  ALock lock(const_cast<ASync_CriticalSection *>(&m_SynchObjectThreadPool));
  for (THREADS::iterator it = m_Threads.begin(); it != m_Threads.end(); ++it)
    (*it)->setRun(isRun);
}

void AThreadPool::stop()
{
  m_CreateNewThreads = false;

  // Signal threads to stop
  setRunStateOnThreads(false);
  m_ThreadPoolTimer.stop();

  // Wait a bit for threads to stop
  int retry1 = 30;
  while (retry1 && m_Threads.size() > 0)
  {
    AThread::sleep(DEFAULT_STOP_CYCLE_SLEEP);
    --retry1;
  }
  
  // Stop monitor and wait for it to exit
  m_MonitorThread.setRun(false);
  int retry2 = 10;
  while (retry2 && m_MonitorThread.isRunning())
  {
    AThread::sleep(DEFAULT_STOP_CYCLE_SLEEP);
    --retry2;
  }
  if (!retry2 && m_MonitorThread.isRunning())
  {
    m_MonitorThread.terminate();
    m_ThreadPoolTimer.stop();      // Only needed when a monitor thread is terminated
  }

  if (!retry1 && m_Threads.size() > 0)
  {
    ALock lock(m_SynchObjectThreadPool);
    // Now that monitor is dead, kill anything that is still alive
    for (THREADS::iterator it = m_Threads.begin(); it != m_Threads.end(); ++it)
    {
      (*it)->terminate();
      delete (*it);
    }
    m_Threads.clear();
  }
}

void AThreadPool::setThreadCount(size_t count)
{
  ALock lock(m_SynchObjectThreadPool);
  m_DesiredThreadCount = count;
}

void AThreadPool::setThis(ABase *p)
{
  ALock lock(m_SynchObjectThreadPool);
  mp_This = p;
}

void AThreadPool::setParameter(ABase *p)
{
  ALock lock(m_SynchObjectThreadPool);
  mp_Parameter = p;
}

ABase *AThreadPool::getThis()
{
  return mp_This;
}

ABase *AThreadPool::getParameter()
{
  return mp_Parameter;
}

size_t AThreadPool::getRunningThreadCount()
{
  size_t ret = 0;

  ALock lock(m_SynchObjectThreadPool);
  for (THREADS::const_iterator cit = m_Threads.begin(); cit != m_Threads.end(); ++cit)
    if ((*cit)->isRunning())
      ++ret;

  return ret;
}

size_t AThreadPool::getThreadCount() const
{
  return m_DesiredThreadCount;
}

size_t AThreadPool::getActiveThreadCount()
{
  size_t ret = 0;
  {
    ALock lock(m_SynchObjectThreadPool);
    ret = m_Threads.size();
  }
  return ret;
}

size_t AThreadPool::getMonitorCycleSleep() const
{
  return m_MonitorCycleSleep;
}

void AThreadPool::setMonitorCycleSleep(size_t sleeptime)
{
  m_MonitorCycleSleep = sleeptime;
}

const AThreadPool::THREADS& AThreadPool::getThreads() const
{
  return m_Threads;
}

ASynchronization& AThreadPool::useSync()
{
  return m_SynchObjectThreadPool;
}

void AThreadPool::setTotalThreadCreationCount(size_t count)
{
  ALock lock(m_SynchObjectThreadPool);
  m_TotalThreadCreationCount = count;
}

size_t AThreadPool::getTotalThreadCreationCount() const
{
  return m_TotalThreadCreationCount;
}

const ATimer& AThreadPool::getThreadPoolTimer() const
{
  return m_ThreadPoolTimer;
}

ATimer& AThreadPool::useThreadPoolTimer()
{
  return m_ThreadPoolTimer;
}

void AThreadPool::waitForThreadsToExit(size_t sleepTime)
{
  while (this->m_MonitorThread.isRunning())
    AThread::sleep(sleepTime);
}

size_t AThreadPool::getTotalThreadsCreated() const
{
  return m_TotalThreadsCreated;
}
