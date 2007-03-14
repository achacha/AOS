#ifndef __AOSModule_Admin_HPP__
#define __AOSModule_Admin_HPP__

#include "AOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSModule_Admin : public AOSModuleInterface
{
public:
  AOSModule_Admin(ALog&);
  virtual bool execute(AContext&);
  
  static const AString MODULE_NAME; //"AOS_Admin"
  virtual const AString& getName() const { return MODULE_NAME; }

private:
  bool __isAuthenticated(AContext& context);
  void __commandShutdown(AContext& context);
  void __commandSetConfiguration(AContext& context);
};

#endif //__AOSModule_PublishInput_HPP__
