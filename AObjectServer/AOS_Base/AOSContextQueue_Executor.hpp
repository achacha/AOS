#ifndef INCLUDED__AOSContextQueue_Executor_HPP__
#define INCLUDED__AOSContextQueue_Executor_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AOSContextQueueThreadPool_RoundRobinSwarm.hpp"

class AOSContext;

#define BASECLASS_AOSContextQueue_Executor AOSContextQueueThreadPool_RoundRobinSwarm

class AOS_BASE_API AOSContextQueue_Executor : public BASECLASS_AOSContextQueue_Executor
{
public:
  /*!
  ctor
  */
  AOSContextQueue_Executor(
    AOSServices&,
    size_t threadCount = 12,
    size_t queueCount = 3
  );

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

protected:
  //a_The main thread proc
  u4 _threadproc(AThread&);
};

#endif //INCLUDED__AOSContextQueue_Executor_HPP__
