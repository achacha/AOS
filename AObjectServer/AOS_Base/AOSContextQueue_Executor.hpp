#ifndef INCLUDED__AOSContextQueue_Executor_HPP__
#define INCLUDED__AOSContextQueue_Executor_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AOSContextQueueThreadPool_RoundRobinSwarm.hpp"
#include "AOSInputExecutor.hpp"
#include "AOSModuleExecutor.hpp"
#include "AOSOutputExecutor.hpp"

class AOSContext;

#define BASECLASS_AOSContextQueue_Executor AOSContextQueueThreadPool_RoundRobinSwarm

class AOS_BASE_API AOSContextQueue_Executor : public BASECLASS_AOSContextQueue_Executor
{
public:
  AOSContextQueue_Executor(
    AOSServices&,
    size_t threadCount = 12,
    size_t queueCount = 3,
    AOSContextQueueInterface *pYes = NULL, 
    AOSContextQueueInterface *pNo = NULL,
    AOSContextQueueInterface *pError = NULL
  );

  /*!
  AOSAdminInterface
  **/
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  AOSInputExecutor
  */
  AOSInputExecutor& useAOSInputExecutor();

  /*!
  AOSModuleExecutor
  **/
  AOSModuleExecutor& useAOSModuleExecutor();

  /*!
  AOSOutputExecutor
  **/
  AOSOutputExecutor& useAOSOutputExecutor();

protected:
  //a_The main thread proc
  u4 _threadproc(AThread&);

  //a_Input executor
  AOSInputExecutor m_InputExecutor;

  //a_Module executor
  AOSModuleExecutor m_ModuleExecutor;

  //a_Input executor
  AOSOutputExecutor m_OutputExecutor;
};

#endif //INCLUDED__AOSContextQueue_Executor_HPP__
