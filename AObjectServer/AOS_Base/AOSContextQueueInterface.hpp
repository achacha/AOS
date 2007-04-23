#ifndef INCLUDED__AOSContextQueueInterface_HPP__
#define INCLUDED__AOSContextQueueInterface_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ASync_CriticalSectionSpinLock.hpp"

class AOSServices;
class AOSContext;
class AOSContextQueueSet;

class AOS_BASE_API AOSContextQueueInterface : public AOSAdminInterface
{
public:
  /*!
  pForward - Forward queue set for AOSContext* (if applicable)
  pBack - Go back (if applicable)
  **/
  AOSContextQueueInterface(
    AOSServices& services,
    AOSContextQueueInterface *pForward = NULL, 
    AOSContextQueueInterface *pBack = NULL,
    AOSContextQueueInterface *pError = NULL
  );
  virtual ~AOSContextQueueInterface();
    
  /*!
  Externally add AOSContext* synchronously to this queue for processing
  **/
  virtual void add(AOSContext *) = 0;

  /*!
  Queue chain
  **/
  void setYesContextQueue(AOSContextQueueInterface *);
  void setNoContextQueue(AOSContextQueueInterface *);
  void setErrorContextQueue(AOSContextQueueInterface *);

  /*!
  AOSAdminInterface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual void processAdminAction(AXmlElement& eBase, const AHTTPRequestHeader& request);

protected:
  /*!
  AOSServices
  */
  AOSServices& m_Services;

  /*!
  Queue sets for chaining based ona simple test
  */
  AOSContextQueueInterface *mp_Yes;
  AOSContextQueueInterface *mp_No;
  AOSContextQueueInterface *mp_Error;

  /*!
  Synchronized remove method used by threadproc to perform work
  **/
  virtual AOSContext *_nextContext() = 0;

  /*!
  Chaining AOSContext*
  if chained queue is NULL, AOSContext::dealloc() is called
  _goTerminate will just deallocate (silent error)
  **/
  virtual void _goYes(AOSContext *);
  virtual void _goNo(AOSContext *);
  virtual void _goError(AOSContext *);
  virtual void _goTerminate(AOSContext *);
};

#endif // INCLUDED__AOSContextQueueInterface_HPP__
