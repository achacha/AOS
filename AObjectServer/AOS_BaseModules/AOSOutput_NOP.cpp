#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_NOP.hpp"

const AString& AOSOutput_NOP::getClass() const
{
  static const AString CLASS("NOP");
  return CLASS;
}

AOSOutput_NOP::AOSOutput_NOP(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

bool AOSOutput_NOP::execute(AOSOutputContext& context)
{
  return true;
}
