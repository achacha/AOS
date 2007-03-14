#ifndef INCLUDED__AOSContextQueueThreadPool_HPP__
#define INCLUDED__AOSContextQueueThreadPool_HPP__

#include "apiAOS_Base.hpp"
#include "AThreadPool.hpp"
#include "AOSContextQueueInterface.hpp"

class AOSServices;
class AOSContext;
class AOSContextQueueSet;

class AOS_BASE_API AOSContextQueueThreadPool : public AOSContextQueueInterface
{
public:
  /*!
  pForward - Forward queue set for AOSContext* (if applicable)
  pBack - Go back (if applicable)
  pError - Error handler (if applicable)
  **/
  AOSContextQueueThreadPool(
    AOSServices& services,
    int threadCount = 1,
    AOSContextQueueInterface *pYes = NULL, 
    AOSContextQueueInterface *pNo = NULL,
    AOSContextQueueInterface *pError = NULL
  );
  virtual ~AOSContextQueueThreadPool();

  AThreadPool& useThreadPool() { return m_ThreadPool; }

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);

protected:
  /*!
  Overridden by child class to add behavior to this queue
  */
  virtual u4 _threadproc(AThread&) = 0;

  // The tread pool
  AThreadPool m_ThreadPool;

  // Main wrapper to the thread pool workers
  static u4 _threadprocWrapper(AThread&);
};

#endif // INCLUDED__AOSContextQueueThreadPool_HPP__
