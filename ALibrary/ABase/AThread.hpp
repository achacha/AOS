/*
Written by Alex Chachanashvili

$Id$
*/
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

  @param that other thread to copy and start immediately if the original thread was flagged to start
  */
  AThread(const AThread& that);
  
  /*!
  Associate thread and thread proc and optionally start on creation
  
  Reason for ABase * instead of void * is to allow the use of dynamic_cast<> to verify that you got what you expected

  Thread proc signature:  u4 threadprocName(AThread& thisThread);

  @param pThreadProc to associate with this thread
  @param boolStart if true then thread will start upon creation
  @param pThis is available via getThis()/setThis()  - doesn't have to be 'this' but makes it easier
  @param pParameter is available via getParameter()/setParameter() - this is anything you want to pass to the thread process
  */
  AThread(ATHREAD_PROC *pThreadProc, bool boolStart = false, ABase *pThis = NULL, ABase *pParameter = NULL);
  
  /*!
  dtor
  */
  ~AThread();

  /*!
  Contains pointer to 'this' if the threadproc is a static member of a class and instance is needed by the thread
  Can be anything, AThread does not use this data in any way and is only here for convenience to the thread process

  @param pThis pointer to be available to the threadproc via thisThread.getThis() and is often died to the object that owns the thread
  */
  void setThis(ABase *pThis);
  
  /*!
  Gets the user specified value, can be anything a long as threadproc knows what to do with it

  @return ABase pointer that the user specified at creation or with setThis call
  */
  ABase *getThis() const;


  /*!
  Passing data to the thread main after it starts
   use set before you start the thread to set the parameter
   use get from inside your function to retrieve the parameter
  
  @param pParam some arbitrary parameter to be available for threadproc (can be anything as long a threadproc knows what to do with it)
  */
  void setParameter(ABase *pParam);
  
  /*!
  Gets a user specified parameter, can be anything a long as threadproc knows what to do with it

  @return ABase pointer that the user specified at creation or with setParamater call
  */
  ABase *getParameter() const;

  /*!
  Get the thread id

  @return thread id (OS specific identifier)
  */
  u4 getId() const;
  
  /*!
  Get thread handle available in Microsoft Windows

  @return Microsoft Windows thread handle
  */
#ifdef __WINDOWS__
  HANDLE getHandle() const;
#endif

  /*!
  Start the thread
  
  @throw AException if threadproc is not set or thread already running
  */
  void start();
  
  /*!
  Thread stop
    Calls setRun(false), then waits for isRunning() to be true
    The tread should be start with setRunning(true), then check isRun() (in some loop) and when it is false should exit and setRunning(false)
    Default will wait 6 * 250 = 1.5 seconds then return (or terminate if so specified)

  @param tries of to wait for the thread to exit
  @param sleepTime between tries while waiting for thtead to exit
  @param terminateIfNotStopped if true will call hard terminate if thread does not stop in time (not recommended for well writtent threadprocs)
  @return true if successfully stopped, false if thread is still running
  */
  bool stop(int tries = 6, u4 sleepTime = 250, bool terminateIfNotStopped = false);

  /*!
  Waits until threadproc is done and returns a.k.a.  pthread_join() on *nix
  On windows it polls the process and sleeps inbetween polls for sleepTime milliseconds

  @param sleepTime applicable to windows to sleep between polling if the thread is still alive
  @return exit code when thread exits
  @throw AException if OS error detected or thread not created
  */
  u4 waitForThreadToExit(u4 sleepTime = 50);

  /*!
  Each call to suspend increments the suspend_counter by 1, if suspend_counter > 0, thread is suspended

  @return value of the current suspend_counter count
  @throw AException if the thread object is invalid or operation failed (system call failed)
  */
  u4 suspend();
  
  /*!
  Each call to resume decrements the suspend_counter by 1, when suspend_counter == 0 thread resumes
    setting boolForceResume to true will call resume until the suspend_counter == 0 and force a resume

  @return value of the current suspend_counter count
  @throw AException if the thread object is invalid or operation failed (system call failed)
  */
  u4 resume(bool boolForceResume = false);

  /*!
  Checks if the tread is running or exited

  @return true if the operating system determines that the thread is active
  */
  bool isThreadActive();

  /*!
  True if a thread object was created (false here may be an issue with OS)
  
  @return true if thread has been created
  */
  bool isThreadCreated();

  /*!
  Get the return code for a thread that exited

  @return exit code if a thread has already exited
  @throw AException if thread is running or not started
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
     Run is what the client wants it to do
     Running is what the thread is actually doing at this moment
   
   NOTE: Run is set to whatever you have the start flag set as
  */
  bool isRunning() const;
  void setRunning(bool boolRunning = true);
  bool isRun() const;
  void setRun(bool boolRun = true);

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
  It should be used as the last resort, setRun/isRunning are a cleaner way
    the thread should be allowed to gracefully exit
  */
  void terminate(u4 uExitCode = 0x0);

  /*!
  AEmittable (paarent of ADebugDumpable)
  */
  void emit(AOutputBuffer&) const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*
  Main thread function
  */
  ATHREAD_PROC *mp_ThreadProc;

  /*
  Parameter that will be stored in this object for the user
  */
  ABase *mpv_This;
  ABase *mpv_Parameter;

  /*
  Flags that threads will use for querying
  */
  bool mbool_Run;
  bool mbool_Running;

  /*!
  Thread ID (non zero indicates a created/running thread)
  Thread HANDLE non NULL for valid thread
  */
  u4     mu4_ThreadId;
  HANDLE mh_Thread;

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
  static u4 __stdcall _ThreadProc(void *pThis);
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

