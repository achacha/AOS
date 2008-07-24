/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "AThread_NOP.hpp"
#include "ASystemException.hpp"

void AThread_NOP::debugDump(std::ostream& os, int indent) const
{
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ") {" << std::endl;
  ADebugDumpable::indent(os, indent+1) << "m_Ret=" << m_Ret << std::endl;

  AThread::debugDump(os, indent);

  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

AThread_NOP::AThread_NOP() :
  m_Ret(0)
{
}

AThread_NOP::AThread_NOP(ATHREAD_PROC *pThreadProc, bool boolStart, ABase *pThis, ABase *pParameter) :
  AThread(pThreadProc, false, pThis, pParameter),
  m_Ret(0)
{
  if (boolStart)
    start();
}

AThread_NOP::~AThread_NOP()
{
}

void AThread_NOP::start()
{
  //a_Already started or exited and restart is needed
  if (mh__Thread)
  {
    //a_Thread is already running
    if (isThreadActive())
      ATHROW(this, AException::ProgrammingError);
    else
    {
      mh__Thread = NULL;
    }
  }

  mbool_Run = true;       //a_thread should run
  mbool_Running = false;  //a_threadproc will set to true when it starts and false when stops (one would hope)
  
  AASSERT(this, mp__ThreadProc);
  AASSERT(this, ADebugDumpable::isPointerValid(mp__ThreadProc));
  mh__Thread = (HANDLE)0x1;
  
  //a_Execute callback
  m_Ret = mp__ThreadProc(*this);
  
  mh__Thread    = NULL;
  mu4__ThreadId = 0x0;
}

void AThread_NOP::terminate(u4)
{
  //a_Doesn't really do anything, just resets the object
  mbool_Run = false;
  mbool_Running = false;
  mh__Thread    = NULL;
  mu4__ThreadId = 0x0;
}

bool AThread_NOP::isThreadActive()
{
  //a_If no thread object, then it is not active
  if (!mh__Thread)
    return false;
  else
    return true;
}

u4 AThread_NOP::getExitCode()
{
  return m_Ret;
}

u4 AThread_NOP::suspend()
{
  return 0;
}

u4 AThread_NOP::resume(bool)
{
  return 0;
}

u4 AThread_NOP::waitForThreadToExit(u4)
{
  return 0;
}

void AThread_NOP::emit(AOutputBuffer& target) const
{
  AThread::emit(target);
}
