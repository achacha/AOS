/*
Written by Alex Chachanashvili

$Id: AThreadPool.cpp 317 2009-11-13 23:29:25Z achacha $
*/
#include "pchAPlatform.hpp"
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
    for (const ABase *p = m_Threads.getHead(); NULL != p; p = p->getNext())
    {
      const AThread *pThread = dynamic_cast<const AThread *>(p);
      pThread->debugDump(os, indent+2);
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
  mp_MonitorThread(NULL),
  m_DesiredThreadCount(threadCount),
  m_MonitorCycleSleep(DEFAULT_MONITOR_CYCLE_SLEEP),
  m_CreateNewThreads(true),
  m_TotalThreadCreationCount(AConstant::npos),
  m_TotalThreadsCreated(0)
{
  mp_MonitorThread = new AThread(AThreadPool::_threadprocDefaultMonitor, false, this);

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
  mp_MonitorThread(NULL),
  m_DesiredThreadCount(threadCount),
  m_MonitorCycleSleep(DEFAULT_MONITOR_CYCLE_SLEEP),
  m_CreateNewThreads(true),
  m_TotalThreadCreationCount(AConstant::npos)
{
  mp_MonitorThread = new AThread(threadprocMonitor, false, this);

  // By default 'this' will be to the thread pool
  if (!mp_This)
    mp_This = this;
}

AThreadPool::~AThreadPool()
{
  AASSERT(this, NULL != mp_MonitorThread);
  try
  {
    if (mp_MonitorThread->isRunning())
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
          if (
               pThis->m_CreateNewThreads 
            && pThis->m_TotalThreadCreationCount > 0 
            && pThis->m_Threads.size() < pThis->m_DesiredThreadCount)
          {
            // More
            AASSERT(pThis, pThis->mp_threadproc);
            AASSERT(pThis, ADebugDumpable::isPointerValid(pThis->mp_threadproc));

            AThread *pthread = new AThread(pThis->mp_threadproc, false, pThis->mp_This, pThis->mp_Parameter);
            pThis->m_Threads.pushBack(pthread);
            pthread->start();
            
            // Allow the thread to start
            AThread::sleep(5);                   // For some odd reason creating too many threads in a short time confuses things
            shouldSleepMore = true;
            
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
          ABase *pBase = pThis->m_Threads.useHead();
          while (diff > 0)
          {
            AASSERT(pThis, pBase);
            AThread *pThread = dynamic_cast<AThread *>(pBase);
            pThread->setRun(false);
            shouldSleepMore = true;
            pBase = pBase->useNext();
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
        ABase *pBase = pThis->m_Threads.useHead();
        while (pBase)
        {
          AThread *pThread = dynamic_cast<AThread *>(pBase);
          if (!pThread->isRunning())
          {
            // Thread somehow died, cleanup
            ABase *pKill = pBase;
            pBase = pBase->useNext();
            pThis->m_Threads.remove(pKill);
            delete pKill;
          }
          else if (!pThread->isRun() && pThread->isRunning())
          {
            ABase *pKill = pBase;
            pBase = pBase->useNext();
            dynamic_cast<AThread *>(pKill)->terminate();
            pThis->m_Threads.remove(pKill);
            delete pKill;
          }
          else
          {
            pBase = pBase->useNext();
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
  AASSERT(this, mp_MonitorThread);
  if (!mp_MonitorThread->isThreadActive() || !mp_MonitorThread->isRunning())
  {
    m_CreateNewThreads = true;
    m_TotalThreadsCreated = 0;
    if (!m_TotalThreadCreationCount)
      m_TotalThreadCreationCount = AConstant::npos;
    
    // Start monitor thread and wait until it flags itself as started
    mp_MonitorThread->start();
    while (!mp_MonitorThread->isRunning())
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
  for (ABase *pBase = m_Threads.useHead(); NULL != pBase; pBase = pBase->useNext())
    dynamic_cast<AThread *>(pBase)->setRun(isRun);
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
  mp_MonitorThread->setRun(false);
  int retry2 = 10;
  while (retry2 && mp_MonitorThread->isRunning())
  {
    AThread::sleep(DEFAULT_STOP_CYCLE_SLEEP);
    --retry2;
  }
  if (!retry2 && mp_MonitorThread->isRunning())
  {
    mp_MonitorThread->terminate();
    m_ThreadPoolTimer.stop();      // Only needed when a monitor thread is terminated
  }

  if (!retry1 && m_Threads.size() > 0)
  {
    ALock lock(m_SynchObjectThreadPool);
    // Now that monitor is dead, kill anything that is still alive
    for (ABase *pBase = m_Threads.useHead(); NULL != pBase; pBase = pBase->useNext())
    {
      dynamic_cast<AThread *>(pBase)->terminate();
      delete pBase;
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

bool AThreadPool::isRunning()
{
  size_t runningNow = getRunningThreadCount();
  return (runningNow == m_DesiredThreadCount);
}

size_t AThreadPool::getRunningThreadCount()
{
  size_t ret = 0;

  ALock lock(m_SynchObjectThreadPool);
  for (const ABase *pBase = m_Threads.getHead(); NULL != pBase; pBase = pBase->getNext())
    if (dynamic_cast<const AThread *>(pBase)->isRunning())
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
  while (this->mp_MonitorThread->isRunning())
    AThread::sleep(sleepTime);
}

size_t AThreadPool::getTotalThreadsCreated() const
{
  return m_TotalThreadsCreated;
}
