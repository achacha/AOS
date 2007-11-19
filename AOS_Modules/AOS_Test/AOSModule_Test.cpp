#include "pchAOS_Test.hpp"
#include "AOSModule_Test.hpp"

const AString& AOSModule_Test::getClass() const
{
  static const AString CLASS("Test");
  return CLASS;
}

AOSModule_Test::AOSModule_Test(AOSServices& services) :
  AOSModuleInterface(services)
{
}

bool AOSModule_Test::execute(AOSContext& context, const AXmlElement& params)
{
  context.useModel().addElement("test").addData("This is a test!");

  return true;
}
