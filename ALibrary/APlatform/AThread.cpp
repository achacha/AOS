/*
Written by Alex Chachanashvili

$Id: AThread.cpp 247 2008-09-26 20:19:37Z achacha $
*/
#include "pchAPlatform.hpp"
#include "AThread.hpp"
#include "ASystemException.hpp"
#include "ATime.hpp"

void AThread::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_ThreadProc=" << AString::fromPointer(mp_ThreadProc) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mpv_This=" << AString::fromPointer(mpv_This) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mpv_Parameter=" << AString::fromPointer(mpv_Parameter) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mbool_Run=" << (mbool_Run ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mbool_Running=" << (mbool_Running ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mu4_ThreadId=" << mu4_ThreadId << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mh_Thread=" << AString::fromPointer(mh_Thread) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ExecutionState=" << m_ExecutionState << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_UserTimerTimeout=" << m_UserTimerTimeout << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_BitArray=" << std::endl;
  m_BitArray.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "m_UserTimer=" << std::endl;
  m_UserTimer.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void APLATFORM_API AThread::sleep(u4 uMilliSeconds)
{
  ATime::sleep(uMilliSeconds);
}

//a_This is the main thread wrapper that calls the user's main function
//a_ and cleans up on exit
u4 AThread::_ThreadProc(void *pThis)
{
  AASSERT(NULL, pThis);
  AASSERT(NULL, ADebugDumpable::isPointerValid(pThis));
  AThread& thread = *(AThread*)pThis;

  thread.mbool_ThreadProcExited = false;
  thread.mbool_ThreadProcCalled = true;
  
  u4 uRet = thread.mp_ThreadProc(thread);
  
  thread.mbool_ThreadProcExited = true;
  thread.mu4_ThreadId = 0x0;

  return uRet;
}

AThread::AThread() :
  mp_ThreadProc(NULL),
  mpv_This(NULL),
  mpv_Parameter(NULL),
  mu4_ThreadId(0x0),
  mh_Thread(NULL),
  mbool_ThreadProcCalled(false),
  mbool_ThreadProcExited(false),
  mbool_Running(false),
  mbool_Run(true),
  m_UserTimer(false),
  m_UserTimerTimeout(0)
{
}

AThread::AThread(ATHREAD_PROC *pThreadProc, bool boolStart, ABase *pThis, ABase *pParameter) :
  mp_ThreadProc(pThreadProc),
  mpv_This(pThis),
  mpv_Parameter(pParameter),
  mu4_ThreadId(0x0),
  mh_Thread(NULL),
  mbool_ThreadProcCalled(false),
  mbool_ThreadProcExited(false),
  mbool_Running(false),
  mbool_Run(true),
  m_UserTimer(false),
  m_UserTimerTimeout(0)
{
  if (boolStart)
    start();
}

AThread::AThread(const AThread& that) :
  mp_ThreadProc(that.mp_ThreadProc),
  mpv_This(that.mpv_This),
  mpv_Parameter(that.mpv_Parameter),
  mu4_ThreadId(0x0),
  mh_Thread(NULL),
  mbool_ThreadProcCalled(false),
  mbool_ThreadProcExited(false),
  mbool_Running(false),
  mbool_Run(that.mbool_Run),
  m_UserTimer(false),
  m_UserTimerTimeout(that.m_UserTimerTimeout),
  m_BitArray(that.m_BitArray)
{
  if (that.mbool_Running)
    start();
}

AThread::~AThread()
{
  stop(3, 200);
  if (mh_Thread)
    ::CloseHandle(mh_Thread);
}

void AThread::start()
{
  AASSERT(this, this);
  AASSERT(this, ADebugDumpable::isPointerValid(this));
  if (!mp_ThreadProc)
    ATHROW(this, AException::InvalidObject);

  //a_Already started or exited and restart is needed
  if (mh_Thread)
  {
    //a_Thread is already running
    if (isThreadActive())
      ATHROW(this, AException::ProgrammingError);
    else
    {
      ::CloseHandle(mh_Thread);
      mh_Thread = NULL;
    }
  }

  mbool_Run = true;       //a_thread should run
  mbool_Running = false;  //a_threadproc will set to true when it starts and false when stops (one would hope)
  
  AASSERT(this, mp_ThreadProc);
  AASSERT(this, ADebugDumpable::isPointerValid(mp_ThreadProc));
  mh_Thread = ::CreateThread(NULL, 0x0, (LPTHREAD_START_ROUTINE)AThread::_ThreadProc, this, 0x0, &mu4_ThreadId);
  if (!mh_Thread)
    ATHROW_LAST_OS_ERROR(this);
}

bool AThread::stop(int tries, u4 sleepTime)
{
  setRun(false);
  while (isRunning() && tries>0)
  {
    --tries;
    AThread::sleep(sleepTime);
  }
  if (isRunning())
    return false;
  else
    return true;
}

bool AThread::isThreadActive()
{
  //a_If no thread object, then it is not active
  if (!mh_Thread)
    return false;

  //a_STILL_ACTIVE is returned by exit code if the thread has not exited
  u4 ret;
  if (!::GetExitCodeThread(mh_Thread, &ret))
    ATHROW_LAST_OS_ERROR(this);

  if (STILL_ACTIVE == ret)
    return true;
  else
    return false;
}

bool AThread::isThreadCreated()
{
  if (mh_Thread)
    return true;
  else
    return false;
}

bool AThread::isThreadProcCalled()
{
  return mbool_ThreadProcCalled;
}

bool AThread::isThreadProcExited()
{
  return mbool_ThreadProcExited;
}

u4 AThread::getExitCode()
{
  if (!mh_Thread)
    ATHROW(this, AException::InvalidObject);

  u4 ret;
  if (!::GetExitCodeThread(mh_Thread, &ret))
    ATHROW_LAST_OS_ERROR(this);
  
  //a_Return value for an active thread does not make sense
  if (STILL_ACTIVE == ret)
    ATHROW(this, AException::InvalidReturnValue);

  return ret;
}

u4 AThread::suspend()
{
  if (!mh_Thread)
    ATHROW(this, AException::InvalidObject);

  /*
    The thread's previous suspend count if successful; 0xFFFFFFFF otherwise. 
    If any thread has a suspend count above zero, that thread does not execute.
  */
  u4 ret = ::SuspendThread(mh_Thread);
  if (0xFFFFFFFF == ret)
    ATHROW(this, AException::OperationFailed);

  return ret;
}

u4 AThread::resume(bool boolForceResume /* = false */)
{
  if (!mh_Thread)
    ATHROW(this, AException::InvalidObject);

  /*
    The thread's previous suspend count if successful; 0xFFFFFFFF otherwise.
    If the return value is zero, the current thread was not suspended.
    If the return value is one, the thread was suspended, but is now restarted.
    Any return value greater than one means the thread remains suspended.
  */
  u4 ret = ::ResumeThread(mh_Thread);
  while (boolForceResume && ret > 1 && 0xFFFFFFFF == ret)
    ret = ::ResumeThread(mh_Thread);

  if (0xFFFFFFFF == ret)
    ATHROW(this, AException::OperationFailed);

  return ret;
}

u4 AThread::waitForThreadToExit(u4 sleepTime)
{
  if (!mh_Thread)
    ATHROW(this, AException::InvalidObject);

  u4 ret = 0;
  do
  {
    if (!::GetExitCodeThread(mh_Thread, &ret))
      ATHROW_LAST_OS_ERROR(this);

    AThread::sleep(sleepTime);
  }
  while (STILL_ACTIVE == ret);

  return ret;
}

ABitArray& AThread::useBitArray()
{ 
  return m_BitArray; 
}

const ABitArray& AThread::getBitArray() const 
{ 
  return m_BitArray; 
}

AString& AThread::useExecutionState()
{
  return m_ExecutionState;
}

const AString& AThread::getExecutionState() const
{
  return m_ExecutionState;
}

void AThread::startUserTimer(u4 millisecond_timeout)
{
  m_UserTimer.start();
  m_UserTimerTimeout = millisecond_timeout;
}

bool AThread::isUserTimerTimedOut() const
{
  return (m_UserTimer.getInterval() > m_UserTimerTimeout);
}

void AThread::clearUserTimer()
{
  m_UserTimer.clear();
}

const ATimer& AThread::getUserTimer() const
{
  return m_UserTimer;
}

u4 AThread::getId() const
{ 
  return mu4_ThreadId;
}

#ifdef __WINDOWS__
HANDLE AThread::getHandle() const 
{ 
  return mh_Thread;
}
#endif

void AThread::emit(AOutputBuffer& target) const
{
  target.append(m_ExecutionState);
}

void AThread::setThis(ABase *pThis)
{ 
  mpv_This = pThis; 
}

ABase *AThread::getThis() const
{ 
  return mpv_This; 
}

void AThread::setParameter(ABase *pParam)
{ 
  mpv_Parameter = pParam; 
}

ABase *AThread::getParameter() const
{ 
  return mpv_Parameter; 
}

bool AThread::isRunning() const 
{ 
  return mbool_Running; 
}

void AThread::setRunning(bool boolRunning /* = true */)
{ 
  mbool_Running = boolRunning; 
}

bool AThread::isRun() const
{ 
  return mbool_Run; 
}

void AThread::setRun(bool boolRun /* = true */)
{ 
  mbool_Run = boolRun;
}
