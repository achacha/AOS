/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchAOS_Test.hpp"
#include "AOSModule_TestFail.hpp"

const AString& AOSModule_TestFail::getClass() const
{
  static const AString CLASS("TestFail");
  return CLASS;
}

AOSModule_TestFail::AOSModule_TestFail(AOSServices& services) :
  AOSModuleInterface(services)
{
}

AOSContext::ReturnCode AOSModule_TestFail::execute(AOSContext& context, const AXmlElement& params)
{
  context.useModel().addElement("test").addData("This is a fail test!");

  //a_Module returns a false
  return AOSContext::RETURN_ERROR;
}
