#ifndef INCLUDED__AOSContextQueue_IsAvailable_SingleQueue_HPP__
#define INCLUDED__AOSContextQueue_IsAvailable_SingleQueue_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueThreadPool.hpp"
#include "ASync_CriticalSectionSpinLock.hpp"

class AOS_BASE_API AOSContextQueue_IsAvailable_SingleQueue : public AOSContextQueueThreadPool
{
public:
  /*!
  ctor
  thread count for this queue is always 1
  **/
  AOSContextQueue_IsAvailable_SingleQueue(AOSServices& services);
  virtual ~AOSContextQueue_IsAvailable_SingleQueue();
  
  /*!
  Externally add AOSContext* synchronously to this queue for processing
  **/
  virtual void add(AOSContext *);

  /*!
  Admin interface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void adminProcessAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

protected:
  /*!
  Tries to read HTTP header for a given AOSRequest
  **/
  virtual u4 _threadproc(AThread&);

  /*!
  Synchronized remove method used by threadproc to perform work
  **/
  AOSContext *_nextContext();

private:
  // Queue and synch
  ASync_CriticalSectionSpinLock m_SynchObjectContextContainer;
  typedef std::list<AOSContext *> REQUESTS;
  REQUESTS m_Queue;
  size_t m_AddCounter;
};

#endif // INCLUDED__AOSContextQueue_IsAvailable_SingleQueue_HPP__
