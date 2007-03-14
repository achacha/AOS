#ifndef INCLUDED__AOSContextQueue_InputExecutor_HPP__
#define INCLUDED__AOSContextQueue_InputExecutor_HPP__

#include "apiAOS_Base.hpp"
#include "AThread.hpp"
#include "AOSAdminInterface.hpp"
#include "AOSSessionManager.hpp"
#include "AOSRequestQueueInterface.hpp"
#include "AOSContextQueueInterface.hpp"

class ADatabasePool;
class ALog;
class AOSContext;
class AOSServices;
class AOSInputExecutor;
class AOSModuleExecutor;
class AOSOutputExecutor;
class AObjectDatabase;

class AOS_BASE_API AOSContextQueue_InputExecutor :
  public AOSAdminInterface, 
  public AOSContextQueueInterface
{
public:
  AOSContextQueue_InputExecutor(
    AOSServices&,  
    AOSInputExecutor&,
    AOSModuleExecutor&,
    AOSOutputExecutor&,
    AOSContextQueueSet *pForward = NULL, 
    AOSContextQueueSet *pBack = NULL
  );

  /*!
  Externally add AOSContext* synchronously to this queue for processing
  **/
  virtual void add(AOSContext *);

  /*!
  Start 'count' # of executors that will process requests in the ARequestQueue object
  Once this count >0 the executors will begin processing, after creation set this value and work starts
  */
  void setExecutorCount(u1 count = 3);

  /*!
  Stop dispatching, flags threads to stop running
  */
  void stopDispatching();
  bool isRunning() const;

  //Thread bits
  enum eTHREADSTATE
  {
    BITNUM_IDLE = 0,            //a_Thread is idle/busy
    BITNUM_WALKSTACK = 1        //a_Thread needs to walk the stack on next admin view
  };

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request) const;
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  enum ENUM_THREAD_STATE
  {
    eNone          = 0x00000000,
    eStarting      = 0x00000001,
    eExit          = 0x00001000
  };

protected:
  /*!
  Synchronized remove method used by threadproc to perform work
  **/
  virtual AOSContext *_nextContext();

  //a_Executor thread proc
  static u4 _threadprocExecutor(AThread&);

private:
  //a_Should not be used as the request is not a context (throws exception)
  virtual AOSRequest *_nextRequest();

  //a_Called by executor to process a request
  bool _processRequest(AOSContext&);

  //a_Dispatcher thread controls executor count
  AThread m_threadDispatcher;
  static u4 _threadprocDispatcher(AThread& thread);
  typedef std::list<AThread *> EXECUTORS;
  EXECUTORS m_Executors;

//  AOSContextQueueInterface& m_RequestQueue;
  AOSInputExecutor& m_InputExecutor;
  AOSModuleExecutor& m_ModuleExecutor;
  AOSOutputExecutor& m_OutputExecutor;
  
  AOSServices& m_Services;

  //a_Executor count (can differ than # of running executors when cleanup thread is active and waiting to remove extras)
  int m_executorCount;

  //a_Session manager
  AOSSessionManager m_SessionManager;
};

#endif //INCLUDED__AOSContextQueue_InputExecutor_HPP__
