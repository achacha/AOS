#include "pchAOS_Test.hpp"
#include "AOSModule_SessionTest.hpp"

const AString& AOSModule_SessionTest::getClass() const
{
  static const AString CLASS("SessionTest");
  return CLASS;
}

AOSModule_SessionTest::AOSModule_SessionTest(ALog& alog) :
  AOSModuleInterface(alog)
{
}

bool AOSModule_SessionTest::execute(AOSContext& context, const AXmlElement& params)
{
  AString str(128, 256);

  //a_Session counter
  AStringHashMap& sessionData = context.useSessionData();
  if (sessionData.get(ASW("count",5), str))
  {
    ANumber n(str);
    ++n;
    str.clear();
    n.emit(str);
    sessionData.set(ASW("count",5), str);
    context.useOutputRootXmlElement().addElement(ASW("/test/count",11), str);
  }
  else
  {
    sessionData.set(ASW("count",5), AString::sstr_Zero);
    context.useOutputRootXmlElement().addElement(ASW("/test/count",11), AString::sstr_Zero);
  }

  return true;
}
