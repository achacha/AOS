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
  ctor
  */
  AOSContextQueueInterface(AOSServices& services);
  virtual ~AOSContextQueueInterface();
    
  /*!
  Externally add AOSContext* synchronously to this queue for processing
  */
  virtual void add(AOSContext *) = 0;

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
  Synchronized remove method used by threadproc to perform work
  **/
  virtual AOSContext *_nextContext() = 0;
};

#endif // INCLUDED__AOSContextQueueInterface_HPP__
