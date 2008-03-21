#include "pchABase.hpp"
#include "AThreadPool.hpp"
#include "ASystemException.hpp"
#include "ALock.hpp"

#define DEFAULT_MONITOR_CYCLE_SLEEP 1000

void AThreadPool::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(AThreadPool @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_monitorCycleSleep=" << m_monitorCycleSleep << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "mp_threadproc=" << AString::fromPointer(mp_threadproc) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_This=" << AString::fromPointer(mp_This) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Parameter=" << AString::fromPointer(mp_Parameter) << std::endl;

  ADebugDumpable::indent(os, indent+1) << "m_Threads={" << std::endl;
  ADebugDumpable::indent(os, indent+2) << "size()=" << m_Threads.size() << std::endl;
  THREADS::const_iterator cit = m_Threads.begin();
  while (cit != m_Threads.end())
  {
    (*cit)->debugDump(os, indent+2);
    ++cit;
  }
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AThreadPool::AThreadPool(
  AThread::ATHREAD_PROC *threadproc,
  int threadCount,   // = 1
  ABase *pThis,       // = NULL
  ABase *pParameter   // = NULL
) :
  mp_threadproc(threadproc),
  mp_This(pThis),
  mp_Parameter(pParameter),
  m_MonitorThread(AThreadPool::_threadprocMonitor, false),
  m_threadCount(threadCount),
  m_monitorCycleSleep(DEFAULT_MONITOR_CYCLE_SLEEP)
{
  m_MonitorThread.setThis(this);

  //a_By default 'this' will be to the thread pool
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

u4 AThreadPool::_threadprocMonitor(AThread& thread)
{
  AThreadPool *pThis = (AThreadPool *)thread.getThis();
  AASSERT(pThis, pThis);

  bool shouldSleepMore = false;
  thread.setRunning(true);
  try
  {
    do
    {
      size_t threadcount = pThis->m_Threads.size();  //a_all running threads
      if (threadcount < pThis->m_threadCount)
      {
        //a_More
        ALock lock(pThis->m_SynchObjectThreadPool);
        while (threadcount < pThis->m_threadCount)
        {
          AASSERT(pThis, pThis->mp_threadproc);
          AASSERT(pThis, ADebugDumpable::isPointerValid(pThis->mp_threadproc));

          AThread *pthread = new AThread(pThis->mp_threadproc, true, pThis->mp_This, pThis->mp_Parameter);
          pThis->m_Threads.push_back(pthread);
          shouldSleepMore = true;
          ++threadcount;
        }
      }
      else if (threadcount > pThis->m_threadCount)
      {
        //a_Less
        ALock lock(pThis->m_SynchObjectThreadPool);
        size_t diff = threadcount - pThis->m_threadCount;
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

      //a_Short sleep to allow time for threads to shut down gracefully
      if (shouldSleepMore)
      {
        AThread::sleep(pThis->m_monitorCycleSleep);
        shouldSleepMore = false;
      }

      //a_Terminate any thread that has been flagged for stopping and has not stopped yet
      THREADS::iterator it = pThis->m_Threads.begin();
      while (it != pThis->m_Threads.end())
      {
        AThread *p = *it;
        if (!(*it)->isRunning())
        {
          ALock lock(pThis->m_SynchObjectThreadPool);
          //a_Thread somehow died, cleanup
          THREADS::iterator itKill = it;
          ++it;
          pThis->m_Threads.erase(itKill);
          delete p;
        }
        else if (!(*it)->isRun() && (*it)->isRunning())
        {
          ALock lock(pThis->m_SynchObjectThreadPool);
          THREADS::iterator itKill = it;
          ++it;
          (*itKill)->terminate();
          pThis->m_Threads.erase(itKill);
          delete p;
        }
        else
          ++it;
      }

      //a_Short sleep to allow time for threads to shut down gracefully
      AThread::sleep(pThis->m_monitorCycleSleep);
    }
    while (thread.isRun());
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
    m_MonitorThread.start();
  }
}

void AThreadPool::stop()
{
  m_threadCount = 0;
  int retry1 = 10;
  while (retry1 && m_Threads.size() > 0)
  {
    AThread::sleep(m_monitorCycleSleep);
    --retry1;
  }
  
  //a_Stop monitor and wait for it to exit
  m_MonitorThread.setRun(false);
  int retry2 = 3;
  while (retry2 && m_MonitorThread.isRunning())
  {
    AThread::sleep(m_monitorCycleSleep);
    --retry2;
  }
  if (!retry2 && m_MonitorThread.isRunning())
  {
    m_MonitorThread.terminate();
  }

  if (!retry1 && m_Threads.size() > 0)
  {
    ALock lock(m_SynchObjectThreadPool);
    //a_Now that monitor is dead, kill anything that is still alive
    THREADS::iterator it = m_Threads.begin();
    while (it != m_Threads.end())
    {
      (*it)->terminate();
      delete (*it);
    }
    m_Threads.clear();
  }
}

void AThreadPool::setThreadCount(size_t count)
{
  m_threadCount = count;
}

void AThreadPool::setThis(ABase *p)
{
  mp_This = p;
}

void AThreadPool::setParameter(ABase *p)
{
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
  THREADS::const_iterator cit = m_Threads.begin();
  while (cit != m_Threads.end())
  {
    if ((*cit)->isRunning())
      ++ret;

    ++cit;
  }
  return ret;
}

size_t AThreadPool::getThreadCount() const
{
  return m_Threads.size();
}

u4 AThreadPool::getMonitorCycleSleep() const
{
  return m_monitorCycleSleep;
}

void AThreadPool::setMonitorCycleSleep(u4 sleeptime)
{
  m_monitorCycleSleep = sleeptime;
}

const AThreadPool::THREADS& AThreadPool::getThreads() const
{
  return m_Threads;
}

ASynchronization& AThreadPool::useSync()
{
  return m_SynchObjectThreadPool;
}
