#ifndef INCLUDED__AOSModuleExecutor_HPP__
#define INCLUDED__AOSModuleExecutor_HPP__

#include "apiAOS_Base.hpp"
#include "AString.hpp"
#include "AOSAdminInterface.hpp"

class AOSServices;
class AOSModuleInterface;
class AOSContext;

class AOS_BASE_API AOSModuleExecutor : public AOSAdminInterface
{
public:
  AOSModuleExecutor(AOSServices&);
  virtual ~AOSModuleExecutor();

  /*!
  Executes appropriate modules based on AContext::getCommand()
  */
  virtual void execute(AOSContext&);

  /*!
  Register command to a module, the module will report its name (AModuleInterface::getName) and must be unique
  The executor owns the modules and will delete them when done
  NOTE: During register init() is called, if this method is overridden that needs to be accounted for
  */
  virtual void registerModule(AOSModuleInterface *);

  /*!
  Admin interface
  */
  virtual void addAdminXml(AXmlElement& eBase, const AHTTPRequestHeader& request);
  virtual const AString& getClass() const;

private:
  AOSServices& m_Services;

  typedef std::map<AString, AOSModuleInterface *> ModuleContainer;
  ModuleContainer m_Modules;

public:
#ifdef __DEBUG_DUMP__
  virtual void debugDump(std::ostream&, int indent = 0x0) const;
#endif
};

#endif // INCLUDED__AOSModuleExecutor_HPP__
