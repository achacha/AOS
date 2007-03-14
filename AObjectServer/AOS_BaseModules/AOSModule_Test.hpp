#ifndef INCLUDED__AOSModule_Test_HPP__
#define INCLUDED__AOSModule_Test_HPP__

#include "apiAOS_BaseModules.hpp"

class AOS_BASEMODULES_API AOSModule_Test : public AOSModuleInterface
{
public:
  AOSModule_Test(ALog&);
  virtual bool execute(AOSContext&, const AObjectContainer&);
  
  static const AString MODULE_CLASS;
  virtual const AString& getClass() const;
};

#endif //INCLUDED__AOSModule_Test_HPP__
