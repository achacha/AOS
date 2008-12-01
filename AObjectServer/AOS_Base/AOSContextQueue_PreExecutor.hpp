/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSContextQueue_PreExecutor_HPP__
#define INCLUDED__AOSContextQueue_PreExecutor_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueThreadPool_RoundRobinSwarm.hpp"

class AOSRequest;
class AOSContext;

class AOS_BASE_API AOSContextQueue_PreExecutor : public AOSContextQueueThreadPool_RoundRobinSwarm
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSContextQueue_PreExecutor(
    AOSServices&,  
    size_t threadCount = 16,
    size_t queueCount = 4
  );
  virtual ~AOSContextQueue_PreExecutor();

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
  //a_The main execution loop for this queue
  u4 _threadproc(AThread&);

  //a_Serve a static page and deallocate the AOSContext
  bool _processStaticPage(AOSContext *pContext);

private:
  //a_Timeout waiting for HTTP data
  int m_WaitForHttpDataTimeout;
};

#endif //INCLUDED__AOSContextQueue_PreExecutor_HPP__
