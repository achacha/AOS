#ifndef INCLUDED__AOSOutputGeneratorInterface_HPP__
#define INCLUDED__AOSOutputGeneratorInterface_HPP__

#include "apiAOS_Base.hpp"
#include "AOSOutputContext.hpp"
#include "AOSAdminInterface.hpp"

class AOSServices;

class AOS_BASE_API AOSOutputGeneratorInterface : public AOSAdminInterface
{
public:
  AOSOutputGeneratorInterface(AOSServices&);
  virtual ~AOSOutputGeneratorInterface() {}

  /*!
  Initialization routine for the module
  This is called when the module is registered by the executor
  This is where the controls get added
  */
  virtual void init() {}

  /*!
  Generate output based on AContext
  */
  virtual bool execute(AOSOutputContext&) = 0;

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
  /*!
  AOS services
  */
  AOSServices& m_Services;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;
#endif
};

#endif //INCLUDED__AOSOutputGeneratorInterface_HPP__
