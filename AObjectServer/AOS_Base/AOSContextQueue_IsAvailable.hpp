#ifndef INCLUDED__AOSContextQueue_IsAvailable_HPP__
#define INCLUDED__AOSContextQueue_IsAvailable_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContextQueueThreadPool.hpp"
#include "ASync_CriticalSectionSpinLock.hpp"

class AOS_BASE_API AOSContextQueue_IsAvailable : public AOSContextQueueThreadPool
{
public:
  /*!
  thread count for this queue is always 1
  pYes - Forward queue set for AOSContext* (if applicable)
  pNo  - Go back (if applicable)
  **/
  AOSContextQueue_IsAvailable(
    AOSServices& services,
    AOSContextQueueInterface *pYes = NULL, 
    AOSContextQueueInterface *pNo  = NULL
  );
  virtual ~AOSContextQueue_IsAvailable();
  
  /*!
  Externally add AOSContext* synchronously to this queue for processing
  **/
  virtual void add(AOSContext *);

  /*!
  Admin interface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);
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

#endif // INCLUDED__AOSContextQueue_IsAvailable_HPP__
