/*
Written by Alex Chachanashvili

$Id$
*/
#ifndef INCLUDED__AOSModuleExecutor_HPP__
#define INCLUDED__AOSModuleExecutor_HPP__

#include "apiAOS_Base.hpp"
#include "AString.hpp"
#include "AOSAdminInterface.hpp"

class AOSServices;
class AOSModuleInterface;
class AOSContext;
class AOSModules;

class AOS_BASE_API AOSModuleExecutor : public AOSAdminInterface
{
public:
  //! Class name
  static const AString CLASS;

public:
  AOSModuleExecutor(AOSServices&);
  virtual ~AOSModuleExecutor();

  /*!
  Executes appropriate modules based on AOSContext::getCommand() or AOSContext::getDirectoryConfig()
  */
  virtual void execute(AOSContext&, const AOSModules& modules);

  /*!
  Register command to a module, the module will report its name (AModuleInterface::getName) and must be unique
  The executor owns the modules and will delete them when done
  NOTE: During register init() is called, if this method is overridden that needs to be accounted for
  */
  virtual void registerModule(AOSModuleInterface *);

  /*!
  Admin interface
  */
  virtual void adminEmitXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

  /*!
  ADebugDumpable
  */
  virtual void debugDump(std::ostream& os = std::cerr, int indent = 0x0) const;

private:
  AOSServices& m_Services;

  typedef std::map<AString, AOSModuleInterface *> ModuleContainer;
  ModuleContainer m_Modules;
};

#endif // INCLUDED__AOSModuleExecutor_HPP__
