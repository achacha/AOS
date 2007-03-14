#include "pchAOS_Test.hpp"
#include "AOSModule_TestFail.hpp"

const AString& AOSModule_TestFail::getClass() const
{
  static const AString CLASS("TestFail");
  return CLASS;
}

AOSModule_TestFail::AOSModule_TestFail(ALog& alog) :
  AOSModuleInterface(alog)
{
}

bool AOSModule_TestFail::execute(AOSContext& context, const AXmlElement& params)
{
  //a_Module returns a false
  return false;
}
