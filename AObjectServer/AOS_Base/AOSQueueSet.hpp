#ifndef INCLUDED__AOSQueueSet_HPP__
#define INCLUDED__AOSQueueSet_HPP__

#include "apiAOS_Base.hpp"
#include "AThread.hpp"
#include "AOSRequestQueueFactoryInterface.hpp"

template <class T>
class AOS_BASE_API AOSQueueSet
{
public:
  /*!
  AOSRequestQueueFactory object is OWNED and DELETED by this object
  AOSRequesrQueueSet objects are NOT OWNED by this object
  **/
  AOSQueueSet(
    ALog& alog,
    AOSRequestQueueFactoryInterface *pQueueFactory,
    int workersPerQueue = 1, 
    int queueCount = 1,
    AOSQueueSet *pForward = NULL, 
    AOSQueueSet *pBack = NULL
  );
  virtual ~AOSQueueSet();

  /*!
  Add AOSRequest*
  */
  virtual void add(AOSRequest *);

private:
  ALog& m_Log;

  typedef std::vector<AOSRequestQueueInterface *> QUEUES;
  QUEUES m_Queues;
  int m_queueCount;
  volatile long m_currentQueue;

  //a_Factory for creating workers
  AOSRequestQueueFactoryInterface *mp_QueueFactory;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AOSQueueSet_HPP__
