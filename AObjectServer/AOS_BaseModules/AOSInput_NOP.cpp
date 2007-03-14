#include "pchAOS_BaseModules.hpp"
#include "AOSInput_NOP.hpp"

const AString& AOSInput_NOP::getClass() const
{
  static const AString CLASS("NOP");
  return CLASS;
}

AOSInput_NOP::AOSInput_NOP(ALog& alog) :
  AOSInputProcessorInterface(alog)
{
}

bool AOSInput_NOP::execute(AOSContext& context)
{
  return true;
}
