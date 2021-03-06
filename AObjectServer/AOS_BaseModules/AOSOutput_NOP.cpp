/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSOutput_NOP.hpp"

const AString AOSOutput_NOP::CLASS("NOP");

const AString& AOSOutput_NOP::getClass() const
{
  return CLASS;
}

AOSOutput_NOP::AOSOutput_NOP(AOSServices& services) :
  AOSOutputGeneratorInterface(services)
{
}

AOSContext::ReturnCode AOSOutput_NOP::execute(AOSContext& context)
{
  return AOSContext::RETURN_OK;
}
