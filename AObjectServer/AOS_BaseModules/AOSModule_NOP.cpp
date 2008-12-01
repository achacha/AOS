/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_BaseModules.hpp"
#include "AOSModule_NOP.hpp"

const AString AOSModule_NOP::CLASS("NOP");

const AString& AOSModule_NOP::getClass() const
{
  return CLASS;
}

AOSModule_NOP::AOSModule_NOP(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_NOP::execute(AOSContext&, const AXmlElement&)
{
  return AOSContext::RETURN_OK;
}

