#ifndef INCLUDED__AOSContextQueue_ErrorExecutor_HPP__
#define INCLUDED__AOSContextQueue_ErrorExecutor_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueInterface.hpp"
#include "AOSContextQueueThreadPool_RoundRobinSwarm.hpp"
#include "AOSOutputExecutor.hpp"

class AOSContext;

#define BASECLASS_AOSContextQueue_ErrorExecutor AOSContextQueueThreadPool_RoundRobinSwarm

class AOS_BASE_API AOSContextQueue_ErrorExecutor : public BASECLASS_AOSContextQueue_ErrorExecutor
{
public:
  AOSContextQueue_ErrorExecutor(
    AOSServices&,
    int threadCount = 3,
    int queueCount = 2,
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

protected:
  u4 _threadproc(AThread&);
};

#endif //INCLUDED__AOSContextQueue_ErrorExecutor_HPP__
