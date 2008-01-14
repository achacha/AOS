#ifndef INCLUDED__AThread_NOP_HPP__
#define INCLUDED__AThread_NOP_HPP__

#include "apiABase.hpp"
#include "AThread.hpp"
#include "ABitArray.hpp"
#include "AString.hpp"
#include "ATimer.hpp"

/*!
Same behavior as AThread, except NO thread is created and threadproc is executed inline
*/
class ABASE_API AThread_NOP : public AThread
{
public:
  /*!
  Default thread object, no thread proc yet
  */
  AThread_NOP();
  
  /*!
  Associate thread and thread proc and optionally start on creation
  if boolStart == true then thread will start upon creation
  pThis is available via getThis()/setThis()  - doesn't have to be 'this' but makes it easier
  pParameter is available via getParameter()/setParameter() - this is anything you want to pass to the thread process
  */
  AThread_NOP(ATHREAD_PROC *pThreadProc, bool boolStart = false, ABase *pThis = NULL, ABase *pParameter = NULL);
  
  /*!
  dtor
  */
  ~AThread_NOP();

  /*!
  Thread execute proc
  */
  void start();

  /*!
  Checks if the tread is running or exited
  */
  bool isThreadActive();

  /*!
  Get the return code for a thread that exited
  */
  u4 getExitCode();

  /*!
  Do nothing in this class
  */
  void terminate(u4 uExitCode = 0x0);
  u4 waitForThreadToExit(u4 sleepTime = 50);
  u4 suspend();
  u4 resume(bool boolForceResume = false);

  /*!
  AEmittable (paarent of ADebugDumpable)
  */
  void emit(AOutputBuffer&) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  no copy ctor
  */
  AThread_NOP(const AThread_NOP&);
  
  /*
  Return value after start() is called
  */
  u4 m_Ret;
};

#endif
