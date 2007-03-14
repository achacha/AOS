#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_NOP.hpp"

const AString& AOSOutput_NOP::getClass() const
{
  static const AString CLASS("NOP");
  return CLASS;
}

AOSOutput_NOP::AOSOutput_NOP(ALog& alog) :
  AOSOutputGeneratorInterface(alog)
{
}

bool AOSOutput_NOP::execute(AOSOutputContext& context)
{
  return true;
}
