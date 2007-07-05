#ifndef INCLUDED__AOSContextQueue_PreExecutor_HPP__
#define INCLUDED__AOSContextQueue_PreExecutor_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueThreadPool_RoundRobinSwarm.hpp"

class AOSRequest;
class AOSContext;

#define BASECLASS_AOSContextQueue_PreExecutor AOSContextQueueThreadPool_RoundRobinSwarm

class AOS_BASE_API AOSContextQueue_PreExecutor : public BASECLASS_AOSContextQueue_PreExecutor
{
public:
  AOSContextQueue_PreExecutor(
    AOSServices&,  
    size_t threadCount = 16,
    size_t queueCount = 4,
    AOSContextQueueInterface *pYes = NULL, 
    AOSContextQueueInterface *pNo = NULL
  );
  virtual ~AOSContextQueue_PreExecutor();

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

protected:
  //a_The main execution loop for this queue
  u4 _threadproc(AThread&);

  //a_Serve a static page and deallocate the AOSContext
  bool _processStaticPage(AOSContext *pContext);
};

#endif //INCLUDED__AOSContextQueue_PreExecutor_HPP__
