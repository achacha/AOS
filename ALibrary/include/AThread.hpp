#ifndef INCLUDED__AThread_HPP__
#define INCLUDED__AThread_HPP__

#include "apiABase.hpp"
#include "ADebugDumpable.hpp"
#include "ABitArray.hpp"
#include "AString.hpp"
#include "ATimer.hpp"

class ABASE_API AThread : public ADebugDumpable
{
public:
  /*!
  Thread proc: u4 myThreadProc(AThread& thread);
  */
  typedef u4(ATHREAD_PROC)(AThread&);

public:
  /*!
  Default thread object, no thread proc yet
  */
  AThread();
  
  /*!
  copy ctor
  */
  AThread(const AThread&);
  
  /*!
  Associate thread and thread proc and optionally start on creation
  if boolStart == true then thread will start upon creation
  pThis is available via getThis()/setThis()  - doesn't have to be 'this' but makes it easier
  pParameter is available via getParameter()/setParameter() - this is anything you want to pass to the thread process
  */
  AThread(ATHREAD_PROC *pThreadProc, bool boolStart = false, void *pThis = NULL, void *pParameter = NULL);
  
  /*!
  dtor
  */
  ~AThread();

  /*!
  Setting the thread process
  */
  void setProc(ATHREAD_PROC *pThreadProc);
  
  /*!
  Contains pointer to 'this' if the threadproc is a static member of a class and instance is needed by the thread
  Can be anything, AThread does not use this data in any way and is only here for convenience to the thread process
  */
  void  setThis(void *pThis) { mpv__This = pThis; }
  void* getThis() const      { return mpv__This; }


  /*!
  Passing data to the thread main after it starts
   use set before you start the thread to set the parameter
   use get from inside your function to retrieve the parameter
  */
  void  setParameter(void *pParam) { mpv__Parameter = pParam; }
  void* getParameter() const       { return mpv__Parameter; }

  /*!
  Access to thread ID (to differentiate between threads...)
  */
  u4 getId() const;
  HANDLE getHandle() const;

  /*!
  Thread start
  */
  void start();
  
  /*!
  Waits until threadproc is done and returns
  a.k.a.  pthread_join() on *nix
  */
  u4 waitForThreadToExit();

  /*!
  Each call to suspend increments the suspend_counter by 1, if suspend_counter > 0, thread is suspended
  Each call to resume decrements the suspend_counter by 1, when suspend_counter == 0 thread resumes
    setting boolForceResume to true will call resume until the suspend_counter == 0 and force a resume
  Return value is the current suspend_counter count
  Exception will be thrown if the thread object is invalid or operation failed (system call failed)
  */
  u4 suspend();
  u4 resume(bool boolForceResume = false);

  /*!
  Checks if the tread is running or exited
  */
  bool isThreadActive();

  /*!
  True if a thread object was created (false here may be an issue with OS)
  */
  bool isThreadCreated();

  /*!
  Get the return code for a thread that exited
  */
  u4 getExitCode();

  /*!
   Thread state management
    these are here to help you control the thread and available for convenient and need not be used at all
   
   isRunning is to be used by the client to see if the thread is still running
   setRunning should only be set in the thread proc, true at start and false on exit/exception
    isThreadActive can be used also, but it will use OS command to check if the thread is running (bit more brute force)
   
   isRun is for use inside the thread proc, while this is true the thread should execute, when false it should try and exit
   setRun used by the client to signal a thread to stop
   
   Difference between Run and Running:
     Run is what the cleint wants it to do
     Running is what the thread is actually doing at this moment
   
   NOTE: Run is set to whatever you have the start flag set as
  */
  bool isRunning() const { return mbool_Running; }
  void setRunning(bool boolRunning = true) { mbool_Running = boolRunning; }
  bool isRun() const { return mbool_Run; }
  void setRun(bool boolRun = true) { mbool_Run = boolRun; }

  /*!
  Bit flags to be read by a running thread; simple ON/OFF messaging
  It is to be used by the user to communicate between the threads and controller
  The parent app can set a certain bit and the thread to acknowledge the request can clear it
  
  For more advanced communication use get/setParameter() with a pointer to a struct/class/etc

  See: ABitArray class for more info on what it supports

  These are purely for user defined behavior between the thread and the outside world
  So do whatever you want with these flags
  */
  ABitArray& useBitArray();
  const ABitArray& getBitArray() const;

  /*!
  Thread state (set by the tread for external query)
  Optional part which you can set from within a thread and outside world can get a hint as what the thread is doing
  User defined, so just like the bit array above, do what you want with this
  */
  AString& useExecutionState();
  const AString& getExecutionState() const;

  /*!
  This is a timer object that you can start/stop
  It is useful to monitor progress of the tread (especially socket threads)

  Usage:
  thread.startUserTimer(10000);
  // do some possibly blocking operation
  threat.resetUserTimer();                   // reset it so that manager thread does not kill it

  ...
  
  Somewhere else a manager thread checks isUserTimerTimedOut() and if it is either setRun(false) or terminate() can be called
  */
  void startUserTimer(u4 millisecond_timeout);
  bool isUserTimerTimedOut() const;
  void clearUserTimer();
  const ATimer& getUserTimer() const;

  /*!
  Sleep for milliseconds
  */
  static void sleep(u4 uMilliSeconds);

  /*!
  DANGER: Do not use this unless absolutely necessary
  Thread termination function
  It should be used as the last resort, setFlag()/hasFlag() are a cleaner way
    the thread should be allowed to gracefully exit
  */
  void terminate(u4 uExitCode = 0x0);

  /*!
  AEmittable (paarent of ADebugDumpable)
  */
  void emit(AOutputBuffer&) const;

private:
  /*
  Main thread function
  */
  ATHREAD_PROC *mp__ThreadProc;

  /*
  Parameter that will be stored in this object for the user
  */
  void *mpv__This;
  void *mpv__Parameter;

  /*
  Flags that threads will use for querying
  */
  bool mbool_Run;
  bool mbool_Running;

  /*!
  Thread ID (non zero indicates a created/running thread)
  Thread HANDLE non NULL for valid thread
  */
  u4     mu4__ThreadId;
  HANDLE mh__Thread;

  /*
  User controlled variables
  */
  ABitArray m_BitArray;
  AString m_ExecutionState;
  ATimer m_UserTimer;
  u4 m_UserTimerTimeout;

  /*
  Wrapper thread main
  */
  static u4 __stdcall __ThreadProc(void *pThis);

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif

/*!  
// Sample program using AThread
#include "AThread.hpp"
#include "ATimer.hpp"
#include "AFile_IOStream.hpp"
#include "AException.hpp"

u4 threadprocSample(AThread& thread)
{
  thread.setRunning(true);  // Flag thread as starting to run
  thread.useExecutionState().assign("Starting to execute");  // Message that stores what a thread is doing (optional)
  try
  {
    do
    {
      thread.useExecutionState().assign("Doing some work");
      
      // Do some work
      std::cout << "." << std::flush;

      AThread::sleep(500);  // Sleep in a loop to manage CPU utilitzation (optional)
    }
    while (thread.isRun());   // Execute in a loop while needed to run
    thread.useExecutionState().assign("Exited");
  }
  catch(AException& ex)
  {
    thread.useExecutionState().assign(ex.what());
  }
  catch(...)
  {
    thread.useExecutionState().assign("Unknown exception caught");
  }
  thread.setRunning(false); // Flag thread as stopping
  return 0;
}

int main()
{
  std::cout << "Thread created, type 'exit' to signal it to stop." << std::endl;
  AThread thread(threadprocSample, true);

  AFile_IOStream io;
  AString str;
  while(AConstant::npos != io.readLine(str))
  {
    if (str.equalsNoCase("exit"))
      break;

    AThread::sleep(500);
  }

  thread.setRun(false);
  int retries = 10;
  while(retries > 0 && thread.isRunning())
  {
    //a_Wait for the thread to gracefully finish and exit
    //a_AThread::terminate should be avoided at all cost, it's brute force (see Microsoft SDK documentation)
    AThread::sleep(100);  //a_Small sleep value to prevent tight empty loops
    --retries;
  }
  if (!retries && thread.isRunning())
    thread.terminate();  //a_The sledgehammer, thread did not exit in allotted time

  std::cout << "Done." << std::endl;

  return 0;
}
*/

