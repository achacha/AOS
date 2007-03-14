#include "pchAOS_BaseModules.hpp"
#include "AOSModule_NOP.hpp"

const AString& AOSModule_NOP::getClass() const
{
  static const AString MODULE_CLASS("NOP");
  return MODULE_CLASS;
}

AOSModule_NOP::AOSModule_NOP(ALog& alog) :
  AOSModuleInterface(alog)
{
}

bool AOSModule_NOP::execute(AOSContext&, const AXmlElement&)
{
  return true;
}

