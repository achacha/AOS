#ifndef INCLUDED__AOSRequestQueueThreadPool_HPP__
#define INCLUDED__AOSRequestQueueThreadPool_HPP__

#include "apiAOS_Base.hpp"
#include "AThreadPool.hpp"
#include "AOSRequestQueueInterface.hpp"

class AOSRequest;
class AOSRequestQueueSet;

class AOS_BASE_API AOSRequestQueueThreadPool : public AThreadPool, public AOSRequestQueueInterface
{
public:
  /*!
  pForward - Forward queue set for AOSRequest* (if applicable)
  pBack - Go back (if applicable)
  **/
  AOSRequestQueueThreadPool(
    AOSServices& services,
    int threadCount = 1,
    AOSRequestQueueInterface *pForward = NULL, 
    AOSRequestQueueInterface *pBack = NULL
  );
  virtual ~AOSRequestQueueThreadPool();

protected:
  /*!
  Overridden by child class to add behavior to this queue
  */
  virtual u4 _threadproc(AThread&) = 0;

private:
  // Main wrapper to the thread pool workers
  static u4 _threadprocWrapper(AThread&);
};

#endif // INCLUDED__AOSRequestQueueThreadPool_HPP__
