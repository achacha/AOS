/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AThread.hpp"
#include "ASystemException.hpp"

void AThread::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp__ThreadProc=" << AString::fromPointer(mp__ThreadProc) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mpv__This=" << AString::fromPointer(mpv__This) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mpv__Parameter=" << AString::fromPointer(mpv__Parameter) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mbool_Run=" << (mbool_Run ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mbool_Running=" << (mbool_Running ? AConstant::ASTRING_TRUE : AConstant::ASTRING_FALSE) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mu4__ThreadId=" << mu4__ThreadId << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mh__Thread=" << AString::fromPointer(mh__Thread) << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_ExecutionState=" << m_ExecutionState << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_UserTimerTimeout=" << m_UserTimerTimeout << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_BitArray=" << std::endl;
  m_BitArray.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "m_UserTimer=" << std::endl;
  m_UserTimer.debugDump(os, indent+2);
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

//a_Static method
void ABASE_API AThread::sleep(u4 uMilliSeconds)
{
  ::Sleep(uMilliSeconds);
}

//a_This is the main thread wrapper that calls the user's main function
//a_ and cleans up on exit
u4 AThread::__ThreadProc(void *pThis)
{
  AThread& thread = *(AThread*)pThis;
  u4 uRet = thread.mp__ThreadProc(thread);

  thread.mh__Thread    = NULL;
  thread.mu4__ThreadId = 0x0;

  return uRet;
}

AThread::AThread() :
  mp__ThreadProc(NULL),
  mpv__This(NULL),
  mpv__Parameter(NULL),
  mu4__ThreadId(0x0),
  mh__Thread(NULL),
  mbool_Running(false),
  mbool_Run(true),
  m_UserTimer(false),
  m_UserTimerTimeout(0)
{
}

AThread::AThread(ATHREAD_PROC *pThreadProc, bool boolStart, ABase *pThis, ABase *pParameter) :
  mp__ThreadProc(pThreadProc),
  mpv__This(pThis),
  mpv__Parameter(pParameter),
  mu4__ThreadId(0x0),
  mh__Thread(NULL),
  mbool_Running(false),
  mbool_Run(true),
  m_UserTimer(false),
  m_UserTimerTimeout(0)
{
  if (boolStart)
    start();
}

AThread::AThread(const AThread& that) :
  mp__ThreadProc(that.mp__ThreadProc),
  mpv__This(that.mpv__This),
  mpv__Parameter(that.mpv__Parameter),
  mu4__ThreadId(0x0),
  mh__Thread(NULL),
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
  stop(3, 200, true);
  if (mh__Thread)
    ::CloseHandle(mh__Thread);
}

void AThread::setProc(ATHREAD_PROC *pThreadProc)
{
  mp__ThreadProc = pThreadProc;
}

void AThread::start()
{
  if (!mp__ThreadProc)
    ATHROW(this, AException::InvalidObject);

  //a_Already started or exited and restart is needed
  if (mh__Thread)
  {
    //a_Thread is already running
    if (isThreadActive())
      ATHROW(this, AException::ProgrammingError);
    else
    {
      ::CloseHandle(mh__Thread);
      mh__Thread = NULL;
    }
  }

  mbool_Run = true;       //a_thread should run
  mbool_Running = false;  //a_threadproc will set to true when it starts and false when stops (one would hope)
  
  AASSERT(this, mp__ThreadProc);
  AASSERT(this, ADebugDumpable::isPointerValid(mp__ThreadProc));
  mh__Thread = ::CreateThread(NULL, 0x0, (LPTHREAD_START_ROUTINE)mp__ThreadProc, this, 0x0, &mu4__ThreadId);
}

bool AThread::stop(int tries, u4 sleepTime, bool terminateIfNotStopped)
{
  setRun(false);
  while (isRunning() && tries>0)
  {
    --tries;
    AThread::sleep(sleepTime);
  }
  if (isRunning())
  {
    if (!terminateIfNotStopped)
      return false;

    terminate();
  }
  return true;
}

void AThread::terminate(u4 uExitCode)
{
  mbool_Run = false;
  mbool_Running = false;

  if (mh__Thread)
  {
    ::TerminateThread(mh__Thread, uExitCode);
    ::CloseHandle(mh__Thread);
  }

  mh__Thread    = NULL;
  mu4__ThreadId = 0x0;
}

bool AThread::isThreadActive()
{
  //a_If no thread object, then it is not active
  if (!mh__Thread)
    return false;

  //a_STILL_ACTIVE is returned by exit code if the thread has not exited
  u4 ret;
  if (!::GetExitCodeThread(mh__Thread, &ret))
    ATHROW_LAST_OS_ERROR(this);

  if (STILL_ACTIVE == ret)
    return true;
  else
    return false;
}

bool AThread::isThreadCreated()
{
  if (mh__Thread)
    return true;
  else
    return false;
}

u4 AThread::getExitCode()
{
  if (!mh__Thread)
    ATHROW(this, AException::InvalidObject);

  u4 ret;
  if (!::GetExitCodeThread(mh__Thread, &ret))
    ATHROW_LAST_OS_ERROR(this);
  
  //a_Return value for an active thread does not make sense
  if (STILL_ACTIVE == ret)
    ATHROW(this, AException::InvalidReturnValue);

  return ret;
}

u4 AThread::suspend()
{
  if (!mh__Thread)
    ATHROW(this, AException::InvalidObject);

  /*
    The thread's previous suspend count if successful; 0xFFFFFFFF otherwise. 
    If any thread has a suspend count above zero, that thread does not execute.
  */
  u4 ret = ::SuspendThread(mh__Thread);
  if (0xFFFFFFFF == ret)
    ATHROW(this, AException::OperationFailed);

  return ret;
}

u4 AThread::resume(bool boolForceResume /* = false */)
{
  if (!mh__Thread)
    ATHROW(this, AException::InvalidObject);

  /*
    The thread's previous suspend count if successful; 0xFFFFFFFF otherwise.
    If the return value is zero, the current thread was not suspended.
    If the return value is one, the thread was suspended, but is now restarted.
    Any return value greater than one means the thread remains suspended.
  */
  u4 ret = ::ResumeThread(mh__Thread);
  while (boolForceResume && ret > 1 && 0xFFFFFFFF == ret)
    ret = ::ResumeThread(mh__Thread);

  if (0xFFFFFFFF == ret)
    ATHROW(this, AException::OperationFailed);

  return ret;
}

u4 AThread::waitForThreadToExit(u4 sleepTime)
{
  if (!mh__Thread)
    ATHROW(this, AException::InvalidObject);

  u4 ret = 0;
  do
  {
    if (!::GetExitCodeThread(mh__Thread, &ret))
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
  return mu4__ThreadId;
}

HANDLE AThread::getHandle() const 
{ 
  return mh__Thread;
}

void AThread::emit(AOutputBuffer& target) const
{
  target.append(m_ExecutionState);
}

void AThread::setThis(ABase *pThis)
{ 
  mpv__This = pThis; 
}

ABase *AThread::getThis() const
{ 
  return mpv__This; 
}

void AThread::setParameter(ABase *pParam)
{ 
  mpv__Parameter = pParam; 
}

ABase *AThread::getParameter() const
{ 
  return mpv__Parameter; 
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
