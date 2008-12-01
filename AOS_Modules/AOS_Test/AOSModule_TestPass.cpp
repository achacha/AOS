/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Test.hpp"
#include "AOSModule_TestPass.hpp"

const AString AOSModule_TestPass::CLASS("Test.Pass");

const AString& AOSModule_TestPass::getClass() const
{
  return CLASS;
}

AOSModule_TestPass::AOSModule_TestPass(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_TestPass::execute(AOSContext& context, const AXmlElement& params)
{
  context.useModel().addElement("message").addData("Hello World!");

  return AOSContext::RETURN_OK;
}
