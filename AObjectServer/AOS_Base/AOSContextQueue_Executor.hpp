#ifndef INCLUDED__AOSContextQueue_Executor_HPP__
#define INCLUDED__AOSContextQueue_Executor_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AOSContextQueueThreadPool_RoundRobinSwarm.hpp"

class AOSContext;

class AOS_BASE_API AOSContextQueue_Executor : public AOSContextQueueThreadPool_RoundRobinSwarm
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

  //! Start
  virtual void start();
  
  //! Stop
  virtual void stop();

  /*!
  AOSAdminInterface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

protected:
  //a_The main thread proc
  u4 _threadproc(AThread&);
};

#endif //INCLUDED__AOSContextQueue_Executor_HPP__
