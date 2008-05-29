/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSInputProcessorInterface_HPP__
#define INCLUDED__AOSInputProcessorInterface_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContext.hpp"
#include "AOSAdminInterface.hpp"

class AOSServices;

class AOS_BASE_API AOSInputProcessorInterface : public AOSAdminInterface
{
public:
  AOSInputProcessorInterface(AOSServices&);
  virtual ~AOSInputProcessorInterface() {}

  /*!
  Initialization routine for the module
  This is called when the module is registered by the executor
  This is where the controls get added
  */
  virtual void init() {}

  /*!
  Process input and manupulate the request and module namespace as needed
  Publish result to AContext.useElement() base which will be transformed to a result
  */
  virtual AOSContext::ReturnCode execute(AOSContext&) = 0;

  /*!
  De-initialization routine for the module
  This is called by the module executor when it is being destroyed
  */
  virtual void deinit() {}

  /*!
  AOSModuleInterface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

protected:
  /*!
  AOS services
  */
  AOSServices& m_Services;
};

#endif //INCLUDED__AOSInputProcessorInterface_HPP__
