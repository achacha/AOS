/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSContextQueue_ErrorExecutor_HPP__
#define INCLUDED__AOSContextQueue_ErrorExecutor_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AOSContextQueueThreadPool_RoundRobinSwarm.hpp"
#include "AOSOutputExecutor.hpp"

class AOSContext;

class AOS_BASE_API AOSContextQueue_ErrorExecutor : public AOSContextQueueThreadPool_RoundRobinSwarm
{
public:
  /*!
  ctor
  */
  AOSContextQueue_ErrorExecutor(
    AOSServices&,
    size_t threadCount = 3,
    size_t queueCount = 2
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
  u4 _threadproc(AThread&);
};

#endif //INCLUDED__AOSContextQueue_ErrorExecutor_HPP__
