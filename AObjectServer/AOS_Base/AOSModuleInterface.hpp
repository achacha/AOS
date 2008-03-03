#ifndef INCLUDED__AOSModuleInterface_HPP__
#define INCLUDED__AOSModuleInterface_HPP__

#include "apiAOS_Base.hpp"
#include "AOSAdminInterface.hpp"
#include "ABasePtrContainer.hpp"
#include "AOSContext.hpp"

class AOSServices;

class AOS_BASE_API AOSModuleInterface : public AOSAdminInterface
{
public:
  AOSModuleInterface(AOSServices&);
  virtual ~AOSModuleInterface() {}

  /*!
  Initialization routine for the module
  This is called when the module is registered by the executor
  This is where the controls get added
  */
  virtual void init() {}

  /*!
  Process input and manupulate the request and module objects as needed

  @param context of the request
  @param params of the module
  */
  virtual AOSContext::ReturnCode execute(AOSContext& context, const AXmlElement& params) = 0;

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

  /*!
  Every module will have their own container that is shared by all calls to this module class (server maintains 1 instance for all calls)
  It is very important that blocking access here be minimal and it only be used for read-only purposes, locks here can be a significant performance hit
  Initialize in init() call for the module
  Read in execute()
  */
  ABasePtrContainer m_Objects;
};

#endif //INCLUDED__AOSModuleInterface_HPP__
