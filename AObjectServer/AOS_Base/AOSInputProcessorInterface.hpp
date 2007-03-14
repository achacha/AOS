#ifndef INCLUDED__AOSInputProcessorInterface_HPP__
#define INCLUDED__AOSInputProcessorInterface_HPP__

#include "apiAOS_Base.hpp"
#include "AOSContext.hpp"
#include "AOSAdminInterface.hpp"

class ALog;

class AOS_BASE_API AOSInputProcessorInterface : public AOSAdminInterface
{
public:
  AOSInputProcessorInterface(ALog&);
  virtual ~AOSInputProcessorInterface() {}

  /*!
  Initialization routine for the module
  This is called when the module is registered by the executor
  This is where the controls get added
  */
  virtual void init(AOSServices&) {}

  /*!
  Process input and manupulate the request and module namespace as needed
  Publish result to AContext.useElement() base which will be transformed to a result
  */
  virtual bool execute(AOSContext&) = 0;

  /*!
  De-initialization routine for the module
  This is called by the module executor when it is being destroyed
  */
  virtual void deinit() {}

  /*!
  AOSModuleInterface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);

protected:
  ALog& m_Log;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSInputProcessorInterface_HPP__
