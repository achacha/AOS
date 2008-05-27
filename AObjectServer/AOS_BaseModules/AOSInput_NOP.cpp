/*
Written by Alex Chachanashvili

Id: $Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSInput_NOP.hpp"

const AString& AOSInput_NOP::getClass() const
{
  static const AString CLASS("NOP");
  return CLASS;
}

AOSInput_NOP::AOSInput_NOP(AOSServices& services) :
  AOSInputProcessorInterface(services)
{
}

AOSContext::ReturnCode AOSInput_NOP::execute(AOSContext& context)
{
  return AOSContext::RETURN_OK;
}
